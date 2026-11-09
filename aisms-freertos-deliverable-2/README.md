# ACPMS – Autonomous Campus Parking Management System (FreeRTOS)

Deliverable 1 (Dependencies and Utils). A FreeRTOS C program that simulates a 5-bay campus car park
with VIP priority, automatic bay release and thread-safe event logging.

## Files

| File | Purpose |
|---|---|
| `acpms.h` | Shared types (`Vehicle_t`, `Bay_t`, `LogMsg_t`), priorities, constants |
| `acpms.c` | All tasks; each requirement has a `/* 1.x ... */` header comment |
| `main.c` | `main()`, `vApplicationMallocFailedHook`, `vAssertCalled` |
| `FreeRTOSConfig.h` | Kernel configuration (1 ms tick, mutexes, software timers, `xTaskDelayUntil`) |
| `Makefile` | Builds app + FreeRTOS-Kernel POSIX port (`-Wall -Wextra -pthread`) |
| `Dockerfile` | `gcc:13` image, clones FreeRTOS-Kernel `V11.1.0`, builds, runs as non-root |
| `run.sh` | Builds the image, runs it with `--network none`, writes `sample-output.txt` + `acpms_log.txt` |

## Requirement map

| Req | Where (`acpms.c`) | Mechanism |
|---|---|---|
| 1.1 Monitor bay availability | `vMonitorTask` | Periodic (`vTaskDelayUntil`, 1000 ms); logs occupied/free, queue depths and the bay map |
| 1.2 Assign bays | `vAllocatorTask` | Woken by task notification on arrival/release; finds a free bay under the mutex, marks it occupied |
| 1.3 VIP detection + priority | `xDetectVip`, `vGateTask`, `vAllocatorTask` | Plate/permit prefix `VIP-` sets `isVip`; VIPs go to `xVipQueue`; the allocator always calls `xQueueReceive(xVipQueue, …, 0)` before the normal queue |
| 1.4 Automatic release | `vBayTimerCallback`, `vExitTask` | One-shot software timer per bay, started with the vehicle's parking duration; the callback posts the bay to `xReleaseQueue`; the exit task frees it and wakes the allocator |
| 1.5 Logging | `vLog`, `vLoggerTask` | Tasks post `LogMsg_t` records to `xLogQueue`; only the logger task writes to stdout and `acpms_log.txt` |

The shared bay array and statistics are protected by `xBayMutex` (`xSemaphoreCreateMutex`).

Log format: `[HH:MM:SS.mmm] EVENT vehicle=ID bay=Bxx action=...`. The timestamp is the FreeRTOS tick count in ms since start.
Event types are ENTRY, VIP_DETECTED, WAITING, FULL, ASSIGNED, RELEASED, MONITOR, SYSTEM and SUMMARY.

## Scenario (scripted, deterministic)

- 0.5–2.5 s: five vehicles arrive (including `VIP-001`) and the car park becomes full.
- 3.0 s and 3.5 s: `STU-104` and `STF-202` arrive and wait in the normal queue.
- 4.0 s: `VIP-002` arrives and waits in the VIP queue.
- 6.5 s: `VIP-001`'s bay is released. `VIP-002` gets it ahead of the two normal vehicles that arrived earlier.
- 8.0 s and 8.5 s: `STU-105` and `VIP-003` arrive. At 9.0 s `VIP-003` takes the freed bay ahead of `STF-202` and `STU-105`.
- 14 s onwards: vehicles arrive while bays are free, so they are assigned immediately.
- When every vehicle has left, the monitor logs a summary (entries, VIP/normal served, releases, maximum waits). The logger then calls `vTaskEndScheduler()` and `main()` returns 0. There is a hard stop at 30 s.

## Build and run (Docker, Linux POSIX port)

```bash
./run.sh
# equivalent to:
docker build -t acpms-freertos .
docker run --name acpms-run --network none acpms-freertos | tee sample-output.txt
docker cp acpms-run:/out/acpms_log.txt ./acpms_log.txt && docker rm acpms-run
```

The container publishes no ports, has no network and mounts nothing.

## Running in Visual Studio (Windows) – not tested

The application code (`acpms.c`, `acpms.h`, `main.c`) uses only FreeRTOS APIs and standard C (`stdio`, `string`, `stdarg`, `stdlib`).
Only `FreeRTOSConfig.h` and the build files are port-specific. To run it as a Visual Studio console application:

1. Start from the official FreeRTOS Windows simulator demo, `FreeRTOS/Demo/WIN32-MSVC` in the main FreeRTOS repository. It already references the `portable/MSVC-MingW` port.
2. Remove the demo's application sources and add `acpms.c`, `acpms.h` and `main.c`.
3. Use the kernel sources `tasks.c`, `queue.c`, `list.c` and `timers.c`, plus `portable/MSVC-MingW/port.c` and `portable/MemMang/heap_4.c` (or `heap_5.c`, which the demo uses).
4. Keep the demo's `FreeRTOSConfig.h` and copy in the ACPMS settings: `configUSE_MUTEXES 1`, `configUSE_TIMERS 1`, `INCLUDE_xTaskDelayUntil 1`, `configTICK_RATE_HZ 1000`, `configUSE_MALLOC_FAILED_HOOK 1`. Set `configTOTAL_HEAP_SIZE` to at least about 256 KB. On Windows, set `configMINIMAL_STACK_SIZE` to a small value such as 256; the 4096 used here is for the POSIX port.
5. If the demo's `main.c` defines hooks that are also defined here (`vApplicationMallocFailedHook`, `vAssertCalled`), keep only one copy of each.

The Windows port has the same limitation as the POSIX port: tasks should not call Windows or stdio APIs concurrently. ACPMS already avoids this because only the logger task prints.
This path was **not tested on Windows**.

## Notes

- Running natively on macOS (Apple clang, same POSIX port) compiled without warnings and showed correct behaviour up to about 15 s. It then stalled inside the port's signal-based tick handling. The FreeRTOS POSIX port targets Linux ("FreeRTOS simulator for Linux"), so use the container.
- Assumptions: 5 bays; VIP is identified only by the `VIP-` prefix; there are no reserved VIP bays, so priority is given by queue ordering; durations are simulated in real time with 1 tick = 1 ms.
