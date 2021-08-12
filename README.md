# Qt Quick Controls 2

**The Qt Quick Controls 2 module has been moved into qtdeclarative.git.**

## Why?

The motivation for the move is explained here:

https://bugreports.qt.io/browse/QTBUG-79454

There was then a contributor summit session:

https://wiki.qt.io/QtCS2021_-_Testing_upstream_changes_with_downstream_modules

The conclusion from this was that the best solution would be to move qtquickcontrols2 into qtdeclarative.

## Going back in time

To inspect the code in the `6.2` branch prior to its removal, follow these steps:

```bash
$ git checkout -b my-6.2 6.2~1
Switched to a new branch 'my-6.2'
```
