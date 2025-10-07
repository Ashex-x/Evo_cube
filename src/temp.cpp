// Wi-Fi Credentials
const char* ssid = "Your_WiFi_SSID";
const char* password = "Your_WiFi_Password";

// WireGuard Client Configuration
char private_key[] = "YOUR_ESP32_PRIVATE_KEY="; // [Interface] PrivateKey
IPAddress local_ip(10, 0, 0, 2);               // [Interface] Address (IP for ESP32 in the VPN network)

// WireGuard Server Peer Configuration
char public_key[] = "SERVER_PUBLIC_KEY=";      // [Peer] PublicKey
IPAddress endpoint_address(123, 45, 67, 89);   // [Peer] Endpoint IP or DNS
int endpoint_port = 51820;                     // [Peer] Endpoint Port
// Optional: Pre-shared Key
// char preshared_key[] = "YOUR_PRESHARED_KEY=";

void setup() {
  // 1. Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(1000); }
  Serial.println("Connected to WiFi.");

  // 2. Initialize WireGuard
  if (!wg.begin(local_ip, private_key, endpoint_address, public_key, endpoint_port)) {
      Serial.println("Failed to initialize WireGuard interface.");
  }
  Serial.println("WireGuard initialized.");
  Serial.print("VPN IP: ");
  Serial.println(wg.getLocalIP());
}