
# SSH-Monitor

A specialized tool developed as a workaround for environments where standard monitoring tools like `strace` are unavailable. This tool captures SSH protocol data using library preloading when conventional system call tracing isn't possible.

## Background

This tool was developed during a CTF challenge where:

- Standard tools like `strace` were not available
- System had `ptrace_scope = 1` (restricted ptrace capabilities)
- Needed to analyze SSH authentication attempts in a MITM scenario

## How It Works

The tool uses `LD_PRELOAD` to hook into read/write operations, providing similar functionality to:

```bash
strace -f -p <PID> -e trace=write,read -o capture
```

but without requiring `strace` or `ptrace` access.

### Key Features

- Captures both read and write operations
- Works without requiring `strace`/`ptrace`
- Functions in restricted environments
- Real-time protocol monitoring
- Clean, readable output format

### Requirements

- Linux environment
- GCC compiler
- Root access
- Basic development tools:

  ```bash
  apt-get install build-essential
  ```

### Installation

1. **Clone the repository:**

   ```bash
   git clone https://github.com/yourusername/ssh-monitor.git
   cd ssh-monitor
   ```

2. **Compile the shared library:**

   ```bash
   gcc -shared -fPIC monitor.c -o monitor.so -ldl
   ```

### Usage

1. **Prepare the log file:**

   ```bash
   touch /tmp/ssh_capture.log
   chmod 666 /tmp/ssh_capture.log
   ```

2. **Run SSH daemon with the monitor:**

   ```bash
   LD_PRELOAD=/absolute/path/to/monitor.so /usr/sbin/sshd -D
   ```

3. **View the captured data:**

   ```bash
   tail -f /tmp/ssh_capture.log
   ```

#### Output Example

```bash
[Timestamp] read(6, "SSH protocol data")
[Timestamp] write(6, "Username")
[Timestamp] read(6, "Password")
[Timestamp] write(6, "Authentication response")
```

## When to Use This Tool

This tool is particularly useful when:

- Standard monitoring tools (`strace`, `ltrace`, etc.) are unavailable
- System has restricted debugging capabilities
- Need to analyze SSH protocol behavior
- Working in CTF or testing environments
- Standard process tracing is blocked

## Technical Details

The tool works by:

- Hooking `libc` read/write functions
- Capturing data before it reaches the SSH daemon
- Logging operations with timestamps
- Formatting output for readability

## Limitations

- Requires root access
- Must be preloaded with SSH daemon
- Only captures read/write operations
- Needs to be started with the SSH service

## Security Considerations

- For educational/testing purposes only
- Use in controlled environments
- Not for production systems
- Follow responsible disclosure practices
- Respect privacy and security guidelines

## Troubleshooting

Common issues:

- **"Permission denied"**: Run with sudo/root
- **"No output"**: Check log file permissions
- **"Library not loaded"**: Verify absolute path in `LD_PRELOAD`

## Contributing

Contributions are welcome! Please:

- Fork the repository
- Create a feature branch
- Submit a pull request

## License

This project is licensed under the **MIT License**.

## Disclaimer

This tool is for educational purposes only. Users are responsible for ensuring compliance with local laws and regulations regarding network monitoring and security testing.

## Acknowledgments

Originally developed for a CTF challenge where standard monitoring tools were unavailable. Special thanks to the security community for insights into library preloading techniques.
