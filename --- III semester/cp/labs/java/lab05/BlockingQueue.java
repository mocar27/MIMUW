import java.util.LinkedList;

public class BlockingQueue<T> {

    LinkedList<T> elements;
    private final int cap;

    public BlockingQueue(int capacity) {
        this.cap = capacity;
        this.elements = new LinkedList<>();
    }

    public synchronized T take() throws InterruptedException {
        while(elements.size() == 0 ) {
            wait();
        }

        T element = elements.getFirst();
        elements.remove(0);
        notifyAll();
        return element;
    }

    public synchronized void put(T item) throws InterruptedException {
        while(elements.size() == cap) {
            wait();
        }
        elements.add(item);
        notifyAll();
    }

    public synchronized int getSize() {
        return elements.size();
    }

    public int getCapacity() {
        return cap;
    }
}