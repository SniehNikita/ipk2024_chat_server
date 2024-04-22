## Known issues

- UDP message will be retranslated later than specified by server argument: this is due to use of poll function for checking timeouts instead of regular thread solution. There is no way to know how much time passed since poll start, because in case system generates interruptions poll() will skip waiting time and go straight to FD handling, so timeouts are counted only when all sleep time exceeded. This change make program much faster and consumes less system resources. Increased time and memory usage might also be caused by regex usage.
- Regex allocated enormous amounts of memory
- ABNF literals (like %d02.03 ..) are not supported