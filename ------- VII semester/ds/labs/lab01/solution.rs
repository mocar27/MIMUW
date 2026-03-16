pub struct Fibonacci {
    // Add here any fields you need.
    number_1: u128,
    number_2: u128,
}

impl Fibonacci {
    /// Create new `Fibonacci`.
    pub fn new() -> Fibonacci {
        Fibonacci {
            number_1: 0,
            number_2: 1,
        }
    }

    /// Calculate the n-th Fibonacci number.
    ///
    /// This shall not change the state of the iterator.
    /// The calculations shall wrap around at the boundary of u8.
    /// The calculations might be slow (recursive calculations are acceptable).
    pub fn fibonacci(n: usize) -> u8 {
        match n {
            0 => 0,
            1 => 1,
            _ => Fibonacci::fibonacci(n - 1).wrapping_add(Fibonacci::fibonacci(n - 2)),
        }
    }
}

impl Iterator for Fibonacci {
    type Item = u128;

    /// Calculate the next Fibonacci number.
    ///
    /// The first call to `next()` shall return the 0th Fibonacci number (i.e., `0`).
    /// The calculations shall not overflow and shall not wrap around. If the result
    /// doesn't fit u128, the sequence shall end (the iterator shall return `None`).
    /// The calculations shall be fast (recursive calculations are **un**acceptable).
    fn next(&mut self) -> Option<Self::Item> {
        // Current calculated value of fib sequence
        let current = self.number_1;
        
        // Check the u128 overflow
        let next = self.number_1.checked_add(self.number_2)?;

        // If the value is not OF, update the values for next iteration
        if next < self.number_1 { 
            return None; 
        }
        else {
            self.number_1 = self.number_2;
            self.number_2 = next;

            Some(current)
        }
    }
}
