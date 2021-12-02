# smw

## Genernal

A Scalable Middleware,integration Eclipse iceoryx and Fast-DDS.

The main aim is to encapsulate an adaptive middleware,it can deal with relation of communication partners automatically.

Communication on same "Host" will use iceoryx,benefit from it's zero-copy ability.

Communication on different "Host" will use Fast-DDS,benefit from flexible DDS protocol.

Maybe I would support SOME/IP protocol for interoperating with AUTOSAR AP...

## Dependencies

### fmt

https://github.com/fmtlib/fmt

### Fast-DDS

https://github.com/eProsima/Fast-DDS

### Eclipse iceoryx

https://github.com/eclipse-iceoryx/iceoryx

### Google Protobuf

https://developers.google.com/protocol-buffers

