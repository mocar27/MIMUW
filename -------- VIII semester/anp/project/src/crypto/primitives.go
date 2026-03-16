package crypto

import (
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rand"
	"crypto/sha256"
	"log"
	"math/big"
)

const keyLength = 64 // length of the private key in bytes

// An ECDSA public key is a pair of integers (x, y). A signature is a pair of integers (r, s). In this
// project, we represent these pairs of integers as strings of 64 bytes, where the first 32 bytes represent
// the first integer and the second 32 bytes represent the second one.
// This is inherited from the project description pdf.

// GeneratePrivateKey Generate a private key
func GeneratePrivateKey() *ecdsa.PrivateKey {
	privateKey, err := ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
	if err != nil {
		log.Fatalln("Failed to generate private key:", err)
	}
	return privateKey
}

// ExtractPublicKey extracts the public key from a private key
func ExtractPublicKey(privateKey *ecdsa.PrivateKey) *ecdsa.PublicKey {
	publicKey, ok := privateKey.Public().(*ecdsa.PublicKey)
	if !ok {
		log.Fatalln("Failed to extract public key from private key")
	}
	return publicKey
}

// PublicKeyToBytes formats a public key as a string of 64 bytes
func PublicKeyToBytes(publicKey *ecdsa.PublicKey) []byte {
	formatted := make([]byte, 64)
	publicKey.X.FillBytes(formatted[:32])
	publicKey.Y.FillBytes(formatted[32:])
	return formatted
}

// ParsePublicKey parses a public key
func ParsePublicKey(data []byte) *ecdsa.PublicKey {
	if len(data) != 64 {
		log.Fatalln("Invalid public key length, must be 64 bytes")
	}

	var x, y big.Int
	x.SetBytes(data[:32])
	y.SetBytes(data[32:])
	publicKey := ecdsa.PublicKey{
		Curve: elliptic.P256(),
		X:     &x,
		Y:     &y,
	}
	return &publicKey
}

// ComputeSignature computes the signature of a message
func ComputeSignature(privateKey *ecdsa.PrivateKey, data []byte) []byte {
	hashed := sha256.Sum256(data)
	r, s, err := ecdsa.Sign(rand.Reader, privateKey, hashed[:])
	if err != nil {
		log.Fatalln("Failed to compute signature:", err)
	}

	signature := make([]byte, 64)
	r.FillBytes(signature[:32])
	s.FillBytes(signature[32:])

	return signature
}

// VerifySignature verifies a signature of a message
func verifySignatureAux(publicKey *ecdsa.PublicKey, data []byte, signature []byte) (ok bool) {
	var r, s big.Int
	r.SetBytes(signature[:32])
	s.SetBytes(signature[32:])
	hashed := sha256.Sum256(data)
	ok = ecdsa.Verify(publicKey, hashed[:], &r, &s)
	return ok
}

func VerifySignature(publicKey *ecdsa.PublicKey, msg []byte, contentEnd int) bool {
	signatureEnd := contentEnd + keyLength
	return verifySignatureAux(publicKey, msg[:contentEnd], msg[contentEnd:signatureEnd])
}
