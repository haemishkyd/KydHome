# Home Assistant

## Notes on configuration yaml files

## SSL Certificate Information
### Why Do It
In order to ensure a level of security if you open up your Home Assistant (or any other web interface) to the internet, it is a good idea to enable HTTPS, in other words encrypt your interchanges with the internet.
If this is not done any traffic to your web interface including passwords sent over the link or information about the devices (gate controls etc) will be available to a snooper a tech savvy snooper.
### A Basic How To Guide
* Get a dynamic DNS.
* Set up port forwarding on your router to allow traffic on port 80 to go to the correct internal ip address.
* This port forwarding should forward port 80 to port 80 on the internal address.
* Get the certificate software from lets encrypt and run the certificate creation command.

```
$ mkdir certbot
$ cd certbot/
$ wget https://dl.eff.org/certbot-auto
$ chmod a+x certbot-auto
$ ./certbot-auto certonly --standalone \
                          --standalone-supported-challenges http-01 \
                          --email your@email.address \
                          -d hass-example.duckdns.org
```
* --standalone creates a temporary webserver that allows the lets encrypt servers to challenge it (--standalone-supported-challenges http01)
* If the challenge is successful the private key is saved to a specific place on your drive.
* The permissions for these then need to be adjusted to allow home assistant to access them.
* Home Assistant config files then need to reference these files.

**Refer to the [HomeAssistant SSL Certificate](https://home-assistant.io/blog/2015/12/13/setup-encryption-using-lets-encrypt/) for more information on the setup of the certificate. It explains in more detail each of the steps above.**
**The [Lets Encrypt](https://letsencrypt.org/getting-started/) website also has good information on the approach and what to do.**

### Renewing Certificate
The certificate only lasts three months and then it needs to be renewed. This is a simple process. One specific thing to remember: as much as now port 443 is now forwarded to 8123 and port 80 is no longer used, for the renewal the **certbot-auto** program still needs to be visible on port 80.
The command to renew is:

```
./certbot-auto renew --quiet --no-self-upgrade --standalone \
                     --standalone-supported-challenges http-01
```                     
