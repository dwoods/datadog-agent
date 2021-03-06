// Unless explicitly stated otherwise all files in this repository are licensed
// under the Apache License Version 2.0.
// This product includes software developed at Datadog (https://www.datadoghq.com/).
// Copyright 2018 Datadog, Inc.

package healthprobe

import (
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"net"
	"net/http"
	"time"

	"github.com/DataDog/datadog-agent/pkg/status/health"
	"github.com/DataDog/datadog-agent/pkg/util/log"
)

const defaultTimeout = time.Second

// Serve configures and starts the http server for the health check.
// It returns an error if the setup failed, or runs the server in a goroutine.
// Stop the server by cancelling the passed context.
func Serve(ctx context.Context, port int) error {
	if port == 0 {
		return errors.New("port should be non-zero")
	}
	ln, err := net.Listen("tcp", fmt.Sprintf("0.0.0.0:%v", port))
	if err != nil {
		return err
	}

	srv := &http.Server{
		Handler:           healthHandler{},
		ReadTimeout:       defaultTimeout,
		ReadHeaderTimeout: defaultTimeout,
		WriteTimeout:      defaultTimeout,
	}

	go srv.Serve(ln)
	go closeOnContext(ctx, srv)
	return nil
}

func closeOnContext(ctx context.Context, srv *http.Server) {
	// Wait for the context to be canceled
	<-ctx.Done()

	// Shutdown the server, it will close the listener
	timeout, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()
	srv.Shutdown(timeout)
}

type healthHandler struct{}

func (h healthHandler) ServeHTTP(w http.ResponseWriter, _ *http.Request) {
	health, err := health.GetStatusNonBlocking()
	if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}

	if len(health.Unhealthy) > 0 {
		w.WriteHeader(http.StatusInternalServerError)
		log.Debugf("Healthcheck failed on: %v", health.Unhealthy)
	}

	jsonHealth, err := json.Marshal(health)
	if err != nil {
		log.Errorf("Error marshalling status. Error: %v, Status: %v", err, h)
		body, _ := json.Marshal(map[string]string{"error": err.Error()})
		http.Error(w, string(body), 500)
		return
	}

	w.Write(jsonHealth)
}
