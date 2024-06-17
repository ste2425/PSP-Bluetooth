export function bufferToString (buffer) {
    const bytes = new Uint8Array(buffer)
    return bytes.reduce((string, byte) => (string + String.fromCharCode(byte)), "")
}
