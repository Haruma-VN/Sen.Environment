namespace Sen.Script {
    /**
     * JavaScript implementaion of Timer
     */

    export class Clock {
        /**
         * Duration after calculate between two start and stop
         */

        private _duration: number;

        /**
         * Start need to be called before getting the duration
         */

        private _start: number | null;

        /**
         * In constructor, first init all of them as null.
         * And so, the time has never been started or stopped.
         * Current time is frozen.
         */

        public constructor() {
            this._start = null;
            this._duration = 0;
        }

        /**
         * Reset the clock
         */

        public reset(): void {
            this._start = null;
            this._duration = 0;
            return;
        }

        /**
         * In the start, we first check if the current start is null.
         * If it's really not null, should not do anything,
         * because the clock is already started
         */

        public start(): void {
            if (this.is_started) {
                throw new Error(Sen.Kernel.Language.get("clock_has_already_been_started"));
            }
            this._start = Sen.Kernel.Thread.now();
            return;
        }

        /**
         * Here we stop the current clock.
         * But if the current clock is not start,
         * an exception will be thrown.
         */

        public stop(): void {
            if (this.is_stopped) {
                throw new Error(Sen.Kernel.Language.get("clock_has_not_started"));
            }
            this._duration += Sen.Kernel.Thread.now() - this._start!;
            this._start = null;
            return;
        }

        /**
         * Get the current duration
         */

        public get duration(): number {
            return this._duration!;
        }

        /**
         * Get the timer, might be null
         */

        public get start_time(): number | null {
            return this._start;
        }

        /**
         * Check if current clock is started
         */

        public get is_started(): boolean {
            return this._start !== null;
        }

        /**
         * Check if current clock is stopped
         */

        public get is_stopped(): boolean {
            return this._start === null;
        }

        /**
         * Start the current clock as safe mode.
         * It will first check, if its not started,
         * will start it.
         */

        public start_safe(): void {
            if (this.is_stopped) {
                this.start();
            }
            return;
        }

        /**
         * Stop the current clock as safe mode.
         * It will first check, if its not ended,
         * will end it.
         */

        public stop_safe(): void {
            if (this.is_started) {
                this.stop();
            }
            return;
        }
    }
}
