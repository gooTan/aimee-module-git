# Aimee module: git

This is the independent `git` source-ownership repository.

It builds `aimee-module-git` as a separate process for the
server bus. Its generated grant serves exactly the
declared stage event kinds. The boundary adapter returns typed
`capability_absent` until the owned implementation is ported behind it; it never
echoes a request or reports false success.

The daemon admits the process only when its installed absolute executable path,
UID, principal class, principal reference, and event-kind grants match the
installed `.grant` file. Copy that generated grant into each declared daemon
policy directory under `modules.d`.


The descriptor-owned production sources, headers, tests, and documentation are
preserved at their canonical paths so their migration history remains auditable.
