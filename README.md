This is a reproducer of https://www.postgresql.org/message-id/flat/17948-fcace7557e449957%40postgresql.org.
<br>
One could execute `docker-compose up --build` from the root directory to run it.

When this packet exchange occurs between libpq and PG server
```
> BDESS
< 2TDCEZ
```
libpq switches connection into `PGASYNC_IDLE` state, even though it sent two `Sync`s and only got one `ReadyForQuery` in response.<br>
This is problematic, because the very next packet server will send is the second `ReadyForQuery`,<br>
which libpq interprets as a response to the **next** query, when in reality it's the response to the second `Sync` message.

We are seing such `< 2TDCEZ` responses in production, where `E` is `canceling statement due to statement timeout.Fpostgres.c.L3328.RProcessInterrupts`, 
and this error relates to the statement being executed, not to the `Sync` request.<br>
PostgreSQL sending both `C` and `E` for the same statement seems peculiar if not buggy (we've also seen `< 2TDE`),<br>
but nonetheless we think libpq shouldn't accept `E` as a valid response to `S`.
