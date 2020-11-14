# NSM client hello handshake (or at least an approach to design a compatible protocol)

This page tries to describe the way NSM clients do their handshake with each other to exchange the signals they provide.

## The protocol
### Handshake start

The handshake starts as soon as the clients are ready for it.
For this driver software, this means as soon as the `NonSessionHandler` enters the `HELLO_START` state,
which happens after the Non session has been started, the project has been opened, and nsmd has indicated `session_is_loaded`.

Each party has to start a handshake of their own, i.e. the driver and Non-Mixer both have to start the handshake.
There is no need to coordinate both handshakes as each handshake provides different data to the other party.

Handshakes start by broadcasting an NSM hello: `/nsm/server/broadcast /nsm/hello`.
This message contains the client's listener URL, version, client ID etc.
In short, everything the other party might or might not need.
**FIXME** _We need a detailed message description (i.e. which parameters and in which order)._

### Handshake reply: Signal fetch

All clients which consider themselves ready to continue the handshake with the starting client (the starting client does not know about the other clients yet) notify the starting client by sending a `/signal/list` directly to retrieve a list of all signals the starting client provides.
The client shall now send all its signals it provides.

### Signal reply

Each provided signal is sent to the requesting client as a reply to its `/signal/list` message by sending `/reply /signal/list s:path i:min i:max i:default_value` for each provided signal seperately.
The list of signals ends by sending `/reply /signal/list` (kind of the equivalent to a zero-terminated string).

The first part of the handshake is now complete.
If the receiving party intends to send signals to the starting party, it has the possiblity to send `/nsm/server/broadcast /nsm/hello` as well to start the handshake in the opposite direction.
Maybe this has been done before or happens in parallel.
One can't know for sure.

## Nomenclature

  * starting client – The client initiating the handshake
  * requesting client – A client responding to the starting client's handshake by requesting its provided signals
  * listener URL – The URL of the respective client's OSC server which can be used to send messages to the client. This is _not_ the nsmd's URL!
  * signal – A tuple `{path, min, max, default_value}` representing some kind of information or event the client providing the signal can send to connected clients.

## TODOs

  * signal directions – Signals can go in both directions. Currently, the protocol does not support directions yet, but NSM's protocol does.

