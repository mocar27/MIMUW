package crypto

import (
	"crypto/ecdsa"
	"crypto/tls"
	"net/http"
	"time"
)

// SecureClient - HTTPS client (TLS-protected communication)
// Communication with the central server happens over HTTP protected by TLS (HTTPS).
// Use this client for any communication with the server.
func SecureClient() *http.Client {
	return &http.Client{
		Timeout: 10 * time.Second,
		Transport: &http.Transport{
			TLSClientConfig: &tls.Config{
				InsecureSkipVerify: false,
			},
		},
	}
}

func SignMessage(privateKey *ecdsa.PrivateKey, message []byte) []byte {
	signature := ComputeSignature(privateKey, message)
	message = append(message, signature...)
	return message
}
