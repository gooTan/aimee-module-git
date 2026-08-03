#include <aimee/core/event_bus/module_runtime.h>

#include <stdio.h>

static const aimee_module_stage_t stages[] = {
   {7425u, 1u},
};

int main(int argc, char **argv)
{
   if (argc != 2)
   {
      fprintf(stderr, "usage: %s DAEMON_MODULE_BUS_SOCKET\n", argv[0]);
      return 2;
   }
   const aimee_module_process_config_t config = {
       .socket_path = argv[1],
       .module_name = "git",
       .principal_class = 1u,
       .principal_ref = 13u,
       .stages = stages,
       .stage_count = sizeof stages / sizeof stages[0],
       /* Replaced by the repository's owned handler during semantic cutover. */
       .handler = NULL,
   };
   return aimee_module_process_run(&config);
}
