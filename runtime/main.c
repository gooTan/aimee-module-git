#define _POSIX_C_SOURCE 200809L
#include <aimee/core/event_bus/bus_client.h>
#include <aimee/core/event_bus/bus_endpoint.h>
#include <aimee/core/event_bus/module_protocol.h>

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

static volatile sig_atomic_t running = 1;

static void stop(int signal_number)
{
   (void)signal_number;
   running = 0;
}

static uint64_t now_ns(void)
{
   struct timespec now;
   if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
      return 0;
   return (uint64_t)now.tv_sec * 1000000000ULL + (uint64_t)now.tv_nsec;
}

static uint32_t stage_for_kind(uint32_t kind)
{
   switch (kind)
   {
   case 7425u: return 1u;
   default: return 0;
   }
}

static void reply_status(bus_client_t *client, const bus_event_t *event, uint32_t stage_id,
                         uint16_t status, uint64_t trace_id)
{
   uint8_t payload[AIMEE_MODULE_MESSAGE_HEADER_LEN];
   aimee_module_message_t reply = {.operation = AIMEE_MODULE_OP_RESULT,
                                    .status = status,
                                    .stage_id = stage_id ? stage_id : 1,
                                    .trace_id = trace_id};
   if (aimee_module_message_encode(&reply, payload, sizeof payload) != 0)
      (void)bus_client_reply(client, event->frame.event_kind,
                             event->frame.correlation_id, payload, sizeof payload);
}

int main(int argc, char **argv)
{
   if (argc != 2)
   {
      fprintf(stderr, "usage: %s DAEMON_MODULE_BUS_SOCKET\n", argv[0]);
      return 2;
   }
   signal(SIGINT, stop);
   signal(SIGTERM, stop);
   int socket_fd = -1;
   bus_client_t client;
   if (bus_endpoint_connect(argv[1], &socket_fd) != 0 ||
       bus_client_attach_as(socket_fd, &client, 1u, 13u) !=
           BUS_CLIENT_OK)
   {
      perror("git: event-bus attach");
      bus_endpoint_close(&socket_fd);
      return 1;
   }
   bus_endpoint_close(&socket_fd);
   while (running && !bus_client_epoch_changed(&client))
   {
      uint64_t now = now_ns();
      if (now != 0)
         bus_client_heartbeat(&client, now);
      bus_event_t event;
      bus_client_result_t result = bus_client_poll(&client, &event);
      if (result == BUS_CLIENT_EPOCH)
         break;
      if (result == BUS_CLIENT_OK && (event.frame.hdr_flags & BUS_F_REQUEST))
      {
         uint32_t expected_stage = stage_for_kind(event.frame.event_kind);
         aimee_module_message_t request;
         aimee_module_message_result_t decoded = aimee_module_message_decode(
             event.payload, event.payload_len, &request);
         if (expected_stage == 0 || decoded != AIMEE_MODULE_MESSAGE_OK ||
             request.operation != AIMEE_MODULE_OP_INVOKE || request.stage_id != expected_stage)
            reply_status(&client, &event, expected_stage, AIMEE_MODULE_STATUS_INVALID_REQUEST, 0);
         else if (aimee_module_deadline_expired(request.deadline_ns, now))
            reply_status(&client, &event, expected_stage,
                         AIMEE_MODULE_STATUS_DEADLINE_EXCEEDED, request.trace_id);
         else
            /* The repository process is deliberately fail-closed until its owned
             * implementation replaces this generated boundary adapter. Echoing a
             * request would falsely claim the feature stage executed. */
            reply_status(&client, &event, expected_stage,
                         AIMEE_MODULE_STATUS_CAPABILITY_ABSENT, request.trace_id);
      }
      const struct timespec idle = {.tv_sec = 0, .tv_nsec = 1000000};
      nanosleep(&idle, NULL);
   }
   bus_client_detach(&client);
   return 0;
}
