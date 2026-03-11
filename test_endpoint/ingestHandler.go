package main

import (
	"io"
	"log"
	"net/http"
)

func ingestHandler(w http.ResponseWriter, r *http.Request) {

	// Only allow POST
	if r.Method != http.MethodPost {
		http.Error(w, "Method Not Allowed", http.StatusMethodNotAllowed)
		return
	}

	// Read request body
	body, err := io.ReadAll(r.Body)
	if err != nil {
		log.Printf("Error reading request body: %v", err)
		http.Error(w, "Failed to read body", http.StatusBadRequest)
		return
	}
	defer r.Body.Close()

	// Log request metadata
	log.Printf("Received request from %s", r.RemoteAddr)

	// Log body
	log.Printf("Payload: %s", string(body))

	// Respond
	w.WriteHeader(http.StatusOK)
	w.Write([]byte("Received"))
}
