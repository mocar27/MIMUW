/*
 * University of Warsaw
 * Concurrent Programming Course 2022/2023
 * Java Assignment
 *
 * Author: Konrad Iwanicki (iwanicki@mimuw.edu.pl)
 * Implemented and modified by: Konrad Mocarski
 */
package cp2022.solution;

import cp2022.base.Workplace;
import cp2022.base.WorkplaceId;
import cp2022.base.Workshop;

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.Semaphore;


public final class WorkshopFactory {

    public static class WorkShop implements Workshop {

        private final HashMap<WorkplaceId, Workplace> workplaceMap;
        HashMap<WorkplaceId, Thread> occupationMap;
        HashMap<Thread, WorkplaceId> workingMap;
        HashMap<Thread, WorkplaceId> wantsMap;

        HashMap<WorkplaceId, Semaphore> semaphoreMap;
        HashMap<WorkplaceId, Semaphore> semaphoreSwitchToMap;

        private int threadsInWorkshop;
        private boolean isCycle = false;
        CountDownLatch cycleLatch = new CountDownLatch(0);

        Semaphore mutex = new Semaphore(1, true);

        Semaphore canEnterWorkshop;

        WorkShop(Collection<Workplace> workplaces) {
            workplaceMap = new HashMap<>();
            occupationMap = new HashMap<>();

            workingMap = new HashMap<>();
            wantsMap = new HashMap<>();

            semaphoreMap = new HashMap<>();
            semaphoreSwitchToMap = new HashMap<>();

            threadsInWorkshop = 0;
            canEnterWorkshop = new Semaphore(2 * workplaces.size(), true);

            for (Workplace w : workplaces) {
                workplaceMap.putIfAbsent(w.getId(), w);
                occupationMap.putIfAbsent(w.getId(), null);
                semaphoreMap.putIfAbsent(w.getId(), new Semaphore(1, true));
                semaphoreSwitchToMap.putIfAbsent(w.getId(), new Semaphore(0, true));
            }
        }

        // Function to change place from the one that the thread is currently
        // occupying to the one that it wants to switch.
        public void changePlaces(WorkplaceId w) {

            WorkplaceId leavingWorkplace = workingMap.get(Thread.currentThread());
            wantsMap.remove(Thread.currentThread(), w);
            workingMap.replace(Thread.currentThread(), leavingWorkplace, w);
            occupationMap.replace(w, Thread.currentThread());

            boolean doesAnybodyWant = false;
            Thread threadThatWants = null;
            for (Map.Entry<Thread, WorkplaceId> pair : wantsMap.entrySet()) {
                if (pair.getValue() == leavingWorkplace) {
                    doesAnybodyWant = true;
                    threadThatWants = pair.getKey();
                }
            }
            if (doesAnybodyWant) {
                if (semaphoreSwitchToMap.get(workingMap.get(threadThatWants)).availablePermits() == 0) {
                    semaphoreSwitchToMap.get(workingMap.get(threadThatWants)).release();
                }
            } else {
                occupationMap.replace(leavingWorkplace, null);
                if (semaphoreMap.get(leavingWorkplace).availablePermits() == 0) {
                    semaphoreMap.get(leavingWorkplace).release();
                }
            }
        }

        @Override
        public Workplace enter(WorkplaceId wid) {

            try {
                canEnterWorkshop.acquire();

                mutex.acquire();
                threadsInWorkshop++;

                // If the thread is the first one that wants to enter for the given place,
                // it will then enter it, else it is going to wait on the fair semaphore in queue.
                if (occupationMap.get(wid) == null) {
                    semaphoreMap.get(wid).acquire();
                    occupationMap.replace(wid, Thread.currentThread());
                    workingMap.put(Thread.currentThread(), wid);
                }
                mutex.release();
            } catch (InterruptedException e) {
                throw new RuntimeException("panic: unexpected thread interruption");
            }

            try {
                if (Thread.currentThread() != occupationMap.get(wid)) {
                    semaphoreMap.get(wid).acquire();
                    mutex.acquire();
                    occupationMap.replace(wid, Thread.currentThread());
                    workingMap.put(Thread.currentThread(), wid);
                    mutex.release();
                }
            } catch (InterruptedException e) {
                throw new RuntimeException("panic: unexpected thread interruption");
            }

            return workplaceMap.get(wid);
        }

        @Override
        public Workplace switchTo(WorkplaceId wid) {

            try {
                mutex.acquire();
                wantsMap.put(Thread.currentThread(), wid);

                if (occupationMap.get(wid) == Thread.currentThread()) {
                    // If the thread wants to switchTo to workplace that it is
                    // currently occupying, nothing happens
                    wantsMap.remove(Thread.currentThread(), wid);
                    mutex.release();
                } else if (occupationMap.get(wid) == null) {
                    // If the thread wants to switchTo to empty workplace,
                    // we are going to check whether anybody that is waiting on the switchTo semaphore
                    // wants to sit on the place that this thread is leaving
                    semaphoreMap.get(wid).acquire();
                    changePlaces(wid);
                    mutex.release();
                } else {
                    // If the workplace that the thread wants to switchTo is not empty
                    // we are going to check whether this thread is in cycle workplace switching
                    // if it is, then we are going to switch them periodically,
                    // if it is not, then it is going to wait on the semaphore until it can switch places.
                    Thread firstThread = Thread.currentThread();
                    Thread iterationThread = occupationMap.get(wid);
                    WorkplaceId idItWants;
                    isCycle = false;

                    while (!isCycle && wantsMap.containsKey(iterationThread)) {
                        if (iterationThread == firstThread) {
                            isCycle = true;
                        }
                        idItWants = wantsMap.get(iterationThread);
                        iterationThread = occupationMap.get(idItWants);
                    }

                    if (isCycle) {
                        iterationThread = occupationMap.get(wid);
                        int cycleLength = 1;
                        while (iterationThread != firstThread) {
                            cycleLength++;
                            idItWants = wantsMap.get(iterationThread);
                            iterationThread = occupationMap.get(idItWants);
                        }
                        cycleLatch = new CountDownLatch(cycleLength);
                        iterationThread = occupationMap.get(wid);
                        occupationMap.replace(wid, null);

                        while (iterationThread != firstThread) {
                            semaphoreSwitchToMap.get(workingMap.get(iterationThread)).release();
                            idItWants = wantsMap.get(iterationThread);
                            Thread nextThead = occupationMap.get(idItWants);
                            wantsMap.remove(iterationThread, wantsMap.get(iterationThread));
                            occupationMap.replace(idItWants, iterationThread);
                            workingMap.replace(iterationThread, idItWants);
                            iterationThread = nextThead;
                        }
                        wantsMap.remove(firstThread, wid);
                        occupationMap.replace(wid, firstThread);
                        workingMap.replace(firstThread, wid);

                        cycleLatch.countDown();
                        cycleLatch.await();

                        isCycle = false;
                        mutex.release();
                    } else {
                        mutex.release();
                        semaphoreSwitchToMap.get(workingMap.get(Thread.currentThread())).acquire();
                        if (isCycle) {
                            cycleLatch.countDown();
                            cycleLatch.await();
                        } else {
                            mutex.acquire();
                            changePlaces(wid);
                            mutex.release();
                        }
                    }
                }
            } catch (InterruptedException e) {
                throw new RuntimeException("panic: unexpected thread interruption");
            }

            return workplaceMap.get(wid);
        }

        @Override
        public void leave() {

            try {
                mutex.acquire();

                WorkplaceId leavingWorkplace = workingMap.get(Thread.currentThread());
                boolean doesAnybodyWant = false;
                Thread threadThatWants = null;

                // Checking whether anybody wants to sit on Workplace we are leaving
                for (Map.Entry<Thread, WorkplaceId> pair : wantsMap.entrySet()) {
                    if (pair.getValue() == leavingWorkplace) {
                        doesAnybodyWant = true;
                        threadThatWants = pair.getKey();
                    }
                }

                if (doesAnybodyWant) {
                    // If anybody wants to switchTo to the place we are leaving,
                    // so then we release the semaphore for the place that this thread
                    // is currently sitting
                    semaphoreSwitchToMap.get(workingMap.get(threadThatWants)).release();
                } else {
                    // If nobody wants to switchTo to the place we are leaving,
                    // so then we release the enter semaphore
                    occupationMap.replace(leavingWorkplace, null);
                    if (semaphoreMap.get(leavingWorkplace).availablePermits() == 0) {
                        semaphoreMap.get(leavingWorkplace).release();
                    }
                }
                workingMap.remove(Thread.currentThread());

                threadsInWorkshop--;
                if (threadsInWorkshop == 0) {
                    int temp = canEnterWorkshop.availablePermits();
                    canEnterWorkshop.release(2 * workplaceMap.size() - temp);
                }
                mutex.release();

            } catch (InterruptedException e) {
                throw new RuntimeException("panic: unexpected thread interruption");
            }
        }
    }

    public final static Workshop newWorkshop(Collection<Workplace> workplaces) {
        return new WorkShop(workplaces);
    }
}