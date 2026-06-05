import sys

class Process:
    def __init__(self, pid, arrival, burst, priority=0):
        self.pid = pid
        self.arrival = arrival
        self.burst = burst
        self.priority = priority
        self.remaining = burst
        self.start = -1
        self.finish = 0
        self.waiting = 0
        self.turnaround = 0
        self.response = -1

    def reset(self):
        self.remaining = self.burst
        self.start = -1
        self.finish = 0
        self.waiting = 0
        self.turnaround = 0
        self.response = -1


def simulate_fcfs(processes):
    procs = [Process(p.pid, p.arrival, p.burst, p.priority) for p in processes]
    procs.sort(key=lambda p: p.arrival)
    time = 0
    for p in procs:
        if time < p.arrival:
            time = p.arrival
        p.start = time
        p.response = p.start - p.arrival
        time += p.burst
        p.finish = time
        p.turnaround = p.finish - p.arrival
        p.waiting = p.turnaround - p.burst
    return procs


def simulate_sjf(processes):
    procs = [Process(p.pid, p.arrival, p.burst, p.priority) for p in processes]
    procs.sort(key=lambda p: p.arrival)
    ready = []
    finished = []
    time = 0
    idx = 0
    while len(finished) < len(procs):
        while idx < len(procs) and procs[idx].arrival <= time:
            ready.append(procs[idx])
            idx += 1
        if not ready:
            time = procs[idx].arrival
            continue
        ready.sort(key=lambda p: p.burst)
        p = ready.pop(0)
        p.start = time
        p.response = p.start - p.arrival
        time += p.burst
        p.finish = time
        p.turnaround = p.finish - p.arrival
        p.waiting = p.turnaround - p.burst
        finished.append(p)
    return finished


def simulate_srtf(processes):
    procs = [Process(p.pid, p.arrival, p.burst, p.priority) for p in processes]
    procs.sort(key=lambda p: p.arrival)
    ready = []
    finished = []
    time = 0
    idx = 0
    n = len(procs)
    while len(finished) < n:
        while idx < n and procs[idx].arrival <= time:
            ready.append(procs[idx])
            idx += 1
        if not ready:
            time = procs[idx].arrival
            continue
        ready.sort(key=lambda p: p.remaining)
        p = ready[0]
        if p.response == -1:
            p.start = time
            p.response = p.start - p.arrival
        time += 1
        p.remaining -= 1
        if p.remaining == 0:
            p.finish = time
            p.turnaround = p.finish - p.arrival
            p.waiting = p.turnaround - p.burst
            finished.append(p)
            ready.pop(0)
    return finished


def simulate_rr(processes, quantum):
    procs = [Process(p.pid, p.arrival, p.burst, p.priority) for p in processes]
    procs.sort(key=lambda p: p.arrival)
    queue = []
    finished = []
    time = 0
    idx = 0
    n = len(procs)
    while len(finished) < n:
        while idx < n and procs[idx].arrival <= time:
            queue.append(procs[idx])
            idx += 1
        if not queue:
            time = procs[idx].arrival
            continue
        p = queue.pop(0)
        if p.response == -1:
            p.start = time
            p.response = p.start - p.arrival
        run_time = min(quantum, p.remaining)
        time += run_time
        p.remaining -= run_time
        while idx < n and procs[idx].arrival <= time:
            queue.append(procs[idx])
            idx += 1
        if p.remaining == 0:
            p.finish = time
            p.turnaround = p.finish - p.arrival
            p.waiting = p.turnaround - p.burst
            finished.append(p)
        else:
            queue.append(p)
    return finished


def simulate_priority_np(processes):
    procs = [Process(p.pid, p.arrival, p.burst, p.priority) for p in processes]
    procs.sort(key=lambda p: p.arrival)
    ready = []
    finished = []
    time = 0
    idx = 0
    while len(finished) < len(procs):
        while idx < len(procs) and procs[idx].arrival <= time:
            ready.append(procs[idx])
            idx += 1
        if not ready:
            time = procs[idx].arrival
            continue
        ready.sort(key=lambda p: p.priority)
        p = ready.pop(0)
        p.start = time
        p.response = p.start - p.arrival
        time += p.burst
        p.finish = time
        p.turnaround = p.finish - p.arrival
        p.waiting = p.turnaround - p.burst
        finished.append(p)
    return finished


def simulate_priority_p(processes):
    procs = [Process(p.pid, p.arrival, p.burst, p.priority) for p in processes]
    procs.sort(key=lambda p: p.arrival)
    ready = []
    finished = []
    time = 0
    idx = 0
    n = len(procs)
    while len(finished) < n:
        while idx < n and procs[idx].arrival <= time:
            ready.append(procs[idx])
            idx += 1
        if not ready:
            time = procs[idx].arrival
            continue
        ready.sort(key=lambda p: (p.priority, p.arrival))
        p = ready[0]
        if p.response == -1:
            p.start = time
            p.response = p.start - p.arrival
        time += 1
        p.remaining -= 1
        if p.remaining == 0:
            p.finish = time
            p.turnaround = p.finish - p.arrival
            p.waiting = p.turnaround - p.burst
            finished.append(p)
            ready.pop(0)
    return finished


def avg_turnaround(procs):
    return sum(p.turnaround for p in procs) / len(procs)

def avg_waiting(procs):
    return sum(p.waiting for p in procs) / len(procs)

def avg_response(procs):
    return sum(p.response for p in procs) / len(procs)


def _fmt_table(headers, rows):
    col_widths = []
    for ci in range(len(headers)):
        w = len(headers[ci])
        for row in rows:
            w = max(w, len(str(row[ci])))
        col_widths.append(w + 2)
    sep = "+" + "+".join("-" * w for w in col_widths) + "+"
    def fmt_row(vals):
        return "|" + "|".join(str(vals[i]).center(col_widths[i]) for i in range(len(vals))) + "|"
    lines = [sep, fmt_row(headers), sep]
    for row in rows:
        lines.append(fmt_row(row))
    lines.append(sep)
    return "\n".join(lines)

def print_results(name, procs):
    headers = ["PID", "Arrival", "Burst", "Priority", "Start", "Finish",
               "Resp.", "Turnaround", "Waiting"]
    rows = []
    for p in sorted(procs, key=lambda x: x.pid):
        rows.append([p.pid, p.arrival, p.burst, p.priority,
                     p.start, p.finish, p.response, p.turnaround, p.waiting])
    print(f"\n{'='*70}")
    print(f"  {name}")
    print(f"{'='*70}")
    print(_fmt_table(headers, rows))
    print(f"\n  Avg Turnaround: {avg_turnaround(procs):.2f}")
    print(f"  Avg Waiting:    {avg_waiting(procs):.2f}")
    print(f"  Avg Response:   {avg_response(procs):.2f}")

def print_summary(all_results):
    print(f"\n{'='*70}")
    print(f"  SUMMARY COMPARISON")
    print(f"{'='*70}")
    headers = ["Algorithm", "Avg TAT", "Avg Waiting", "Avg Response"]
    rows = []
    for name, procs in all_results:
        rows.append([name,
                     f"{avg_turnaround(procs):.2f}",
                     f"{avg_waiting(procs):.2f}",
                     f"{avg_response(procs):.2f}"])
    print(_fmt_table(headers, rows))


def parse_input(filepath):
    processes = []
    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith('#'):
                continue
            parts = line.split()
            if len(parts) >= 3:
                pid = parts[0]
                arrival = int(parts[1])
                burst = int(parts[2])
                priority = int(parts[3]) if len(parts) >= 4 else 0
                processes.append(Process(pid, arrival, burst, priority))
    return processes


def main():
    import argparse
    parser = argparse.ArgumentParser(description="CPU Scheduler Simulator")
    parser.add_argument("input", nargs="?", default="processes.txt",
                        help="Input file (default: processes.txt)")
    parser.add_argument("-q", "--quantum", type=int, default=2,
                        help="Time quantum for Round Robin (default: 2)")
    parser.add_argument("-a", "--algorithms", nargs="+",
                        choices=["fcfs", "sjf", "srtf", "rr", "priority-np", "priority-p", "all"],
                        default=["all"],
                        help="Algorithms to simulate (default: all)")
    args = parser.parse_args()

    try:
        processes = parse_input(args.input)
    except FileNotFoundError:
        print(f"Error: file '{args.input}' not found.", file=sys.stderr)
        sys.exit(1)

    if not processes:
        print("Error: no processes loaded.", file=sys.stderr)
        sys.exit(1)

    algo_map = {
        "fcfs":        ("FCFS",               simulate_fcfs),
        "sjf":         ("SJF (Non-Preemptive)", simulate_sjf),
        "srtf":        ("SRTF (Preemptive)",   simulate_srtf),
        "rr":          ("Round Robin",         lambda p: simulate_rr(p, args.quantum)),
        "priority-np": ("Priority (Non-Preemptive)", simulate_priority_np),
        "priority-p":  ("Priority (Preemptive)",    simulate_priority_p),
    }

    if "all" in args.algorithms:
        keys = ["fcfs", "sjf", "srtf", "rr", "priority-np", "priority-p"]
    else:
        keys = args.algorithms

    all_results = []
    for key in keys:
        name, func = algo_map[key]
        result = func(processes)
        print_results(name, result)
        all_results.append((name, result))

    print_summary(all_results)


if __name__ == "__main__":
    main()
