package main

import (
	"context"
	"fmt"
	"os"
	"os/signal"
	"syscall"

	"github.com/JBailes/aimee/server-go/bus"
	handler "github.com/JBailes/aimee/server-go/modules/git"
)

func main() {
	if len(os.Args) != 2 {
		fmt.Fprintf(os.Stderr, "usage: %s DAEMON_MODULE_BUS_SOCKET\n", os.Args[0])
		os.Exit(2)
	}
	ctx, stop := signal.NotifyContext(context.Background(), syscall.SIGINT, syscall.SIGTERM)
	defer stop()
	config := bus.ModuleProcessConfig{
		SocketPath: os.Args[1], ModuleName: "git",
		PrincipalClass: 1, PrincipalRef: 13,
		Stages: []bus.ModuleStage{
		{EventKind: 7425, StageID: 1},
		{EventKind: 7426, StageID: 2},
		{EventKind: 7427, StageID: 3},
		{EventKind: 7428, StageID: 4},
		{EventKind: 7429, StageID: 5},
		{EventKind: 7430, StageID: 6},
		},
		Handler: handler.Handle,
	}
	if err := bus.RunModuleProcess(ctx, config); err != nil {
		fmt.Fprintf(os.Stderr, "aimee-module-git: %v\n", err)
		os.Exit(1)
	}
}
