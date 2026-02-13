// ===========================
// TIMER.JS - Performance Tracking
// ===========================

class PerformanceTimer {
    constructor() {
        this.startTime = null;
        this.queryTimes = [];
        this.maxSamples = 50;
    }

    /**
     * Start timing a query
     */
    start() {
        this.startTime = performance.now();
    }

    /**
     * Stop timing and record latency
     * @returns {number} Elapsed time in milliseconds
     */
    stop() {
        if (!this.startTime) return 0;
        
        const elapsed = performance.now() - this.startTime;
        this.queryTimes.push(elapsed);
        
        // Keep only last N samples
        if (this.queryTimes.length > this.maxSamples) {
            this.queryTimes.shift();
        }
        
        this.startTime = null;
        return elapsed;
    }

    /**
     * Get average query latency
     * @returns {number} Average time in milliseconds
     */
    getAverageLatency() {
        if (this.queryTimes.length === 0) return 0;
        
        const sum = this.queryTimes.reduce((a, b) => a + b, 0);
        return sum / this.queryTimes.length;
    }

    /**
     * Get latest query time
     * @returns {number} Most recent time in milliseconds
     */
    getLatestLatency() {
        if (this.queryTimes.length === 0) return 0;
        return this.queryTimes[this.queryTimes.length - 1];
    }

    /**
     * Get performance statistics
     * @returns {Object} Stats object with min, max, avg
     */
    getStats() {
        if (this.queryTimes.length === 0) {
            return { min: 0, max: 0, avg: 0, count: 0 };
        }

        return {
            min: Math.min(...this.queryTimes),
            max: Math.max(...this.queryTimes),
            avg: this.getAverageLatency(),
            count: this.queryTimes.length
        };
    }

    /**
     * Reset all timing data
     */
    reset() {
        this.startTime = null;
        this.queryTimes = [];
    }

    /**
     * Format time for display
     * @param {number} ms - Milliseconds
     * @returns {string} Formatted time string
     */
    static formatTime(ms) {
        if (ms < 1) {
            return `${(ms * 1000).toFixed(0)}μs`;
        } else if (ms < 1000) {
            return `${ms.toFixed(2)}ms`;
        } else {
            return `${(ms / 1000).toFixed(2)}s`;
        }
    }
}

// Create global timer instance
const performanceTimer = new PerformanceTimer();
