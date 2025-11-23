use std::collections::VecDeque;
use std::sync::{Arc, Condvar, Mutex};
use std::sync::atomic::{AtomicBool, Ordering};
use std::thread::JoinHandle;

type Task = Box<dyn FnOnce() + Send>;

// You can define new types (e.g., structs) if you need.
// However, they shall not be public (i.e., do not use the `pub` keyword).

/// The thread pool.
pub struct Threadpool {
    // Add here any fields you need.
    // We suggest storing handles of the worker threads, submitted tasks,
    // and information whether the pool is running or is shutting down.
    workers: Vec<JoinHandle<()>>,
    tasks: Arc<(Mutex<VecDeque<Task>>, Condvar)>,
    running: Arc<AtomicBool>,
}

impl Threadpool {   
    /// Create new thread pool with `workers_count` workers.
    pub fn new(workers_count: usize) -> Self {
        let running: Arc<AtomicBool> = Arc::new(AtomicBool::new(true));
        let mut workers: Vec<JoinHandle<()>> = Vec::with_capacity(workers_count);
        let tasks: Arc<(Mutex<VecDeque<Task>>, Condvar)> = Arc::new((Mutex::new(VecDeque::new()), Condvar::new()));

        for _ in 0..workers_count {
            let rc = running.clone();
            let tc = tasks.clone();
            let worker = std::thread::spawn(move || {
                Self::worker_loop(rc, tc);
            });

            workers.push(worker);
        }

        Threadpool {
            workers,
            tasks,
            running,
        }
    }

    /// Submit a new task.
    pub fn submit(&self, task: Task) {
        // System is shutting down, do not accept new tasks
        if self.running.load(Ordering::SeqCst) == false { // check later Relaxed memory?
            return;
        }

        let (lock, cvar) = &*self.tasks;
        let mut guard = lock.lock().unwrap();

        (*guard).push_back(task);
        cvar.notify_all();
    }

    // We suggest extracting the implementation of the worker to an associated
    // function, like this one (however, it is not a part of the public
    // interface, so you can delete it if you implement it differently):
    fn worker_loop(running: Arc<AtomicBool>, tasks: Arc<(Mutex<VecDeque<Task>>, Condvar)>) {
        loop {
            let (lock, cvar) = &*tasks;
            let mut guard = lock.lock().unwrap();
            
            // Wait for a task.
            while running.load(Ordering::SeqCst) && (*guard).is_empty() {
                guard = cvar.wait(guard).unwrap();
            }

            // If the pool is shutting down and there are no tasks, break the loop.
            // If condition consists of `running` variable, as there can be spurious wakeups.
            if !running.load(Ordering::SeqCst) && (*guard).is_empty() {
                break;
            }

            // Get the task and execute it.
            let task = (*guard).pop_front().unwrap();
            std::mem::drop(guard);
            task();
        }
    }
}

impl Drop for Threadpool {
    /// Gracefully end the thread pool.
    ///
    /// It waits until all submitted tasks are executed,
    /// and until all threads are joined.
    fn drop(&mut self) {
        // Thread pool is to be shut down.
        self.running.store(false, Ordering::SeqCst);

        // Wake up all waiting threads.
        let (_lock, cvar) = &*self.tasks;
        cvar.notify_all();

        // Gracefully end the threads in the pool.
        for worker in self.workers.drain(..) {
            worker.join().unwrap();
        }
    }
}
