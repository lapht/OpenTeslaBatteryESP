# Web access

The page is reachable at three addresses once you join the `OpenTeslaBattery-XXXX` WiFi (password printed on serial at boot):

| Address | How it resolves | Works on |
|---|---|---|
| `http://tesla.lan/` | DNS server running on the ESP | Any client using the DNS it gets from the access point (default) |
| `http://192.168.4.1/` | No name resolution | Always |
| `http://tesla.local/` | mDNS (Bonjour) | iOS, macOS, Windows 10+, Linux with Avahi, Android 12+ in most browsers |

The names are build-time settings (`OTB_DNS_NAME`, `OTB_MDNS_NAME` in `src/config.h`).

## How it works

The ESP is the DHCP server of its own access point, and it hands itself out as the DNS server. A small DNS server (Arduino `DNSServer`) answers on port 53.

With `OTB_CAPTIVE_PORTAL=1` (default) it answers **every** name with `192.168.4.1`, like a hotel WiFi login page:

1. The phone joins the WiFi and checks for internet (`connectivitycheck.gstatic.com/generate_204`, `captive.apple.com/hotspot-detect.html`, `www.msftconnecttest.com/connecttest.txt`...).
2. The check lands on the ESP, which redirects any request for a foreign host to `http://tesla.lan/`.
3. The phone concludes it is behind a captive portal and shows "Sign in to network" (Android) or opens the login sheet (iOS/macOS), which **is** the battery page.

With `OTB_CAPTIVE_PORTAL=0` the DNS server answers only `OTB_DNS_NAME` and returns NXDOMAIN for anything else. No redirect, no automatic popup.

### Why `.lan` and not a real domain

`tesla.lan` only exists inside the ESP's network: nothing is registered, nothing leaves the car. `.lan` is not an official top-level domain, so it can never collide with a real website, but browsers do not know it either. Type the scheme (`http://tesla.lan/`) or the address bar will run a web search instead.

`.local` is reserved for mDNS (RFC 6762) and is resolved by the phone itself, without the ESP's DNS server. It is the fallback when the phone ignores the access point's DNS.

## Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| Browser searches for "tesla.lan" | No scheme typed | Type `http://tesla.lan/` |
| `tesla.lan` does not open, `192.168.4.1` does | Phone is sending DNS over mobile data because the WiFi "has no internet" | Tap the "Sign in to network" notification, or disable mobile data while using the page, or use `tesla.local` |
| Same, on Android with Private DNS set to a hostname | Encrypted DNS bypasses the ESP | Use `tesla.local` or the IP, or set Private DNS to Automatic |
| iOS login sheet closes and WiFi disconnects | "Cancel" in the sheet means "leave this network" | Choose "Use Other Network" > "Use Without Internet", then open the page in Safari |
| No popup at all | `OTB_CAPTIVE_PORTAL=0`, or the OS remembers this network as "no internet" | Open the address manually |

## Security notes

- Every client on the access point gets the ESP's answers for every name while captive mode is on. This is the intended behaviour of a captive portal and has no effect outside the access point.
- Nothing is reachable from the internet: the ESP has no uplink in SoftAP mode.
- Anyone who knows the WiFi password can read the page. See [SECURITY.md](../SECURITY.md).
