# IPK Calculator Protocol Client - Changelog

## Version 1.0

### Functionalities

- Implemented client for IPK Calculator Protocol with two modes: TCP and UDP
- Implemented SIGINT signal handler for TCP mode

### Known Limitations

- UDP mode cannot handle requests with more than 256 characters
