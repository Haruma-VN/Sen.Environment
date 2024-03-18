namespace Sen.Script {
    /**
     * Script implementation of Command Counter to count how much command has been parsed
     */

    export class CommandCounter {
        /**
         * Internal counter variable to track the number of commands.
         */
        private m_count: bigint;

        /**
         * Creates a new `CommandCounter` instance with the initial count set to 0.
         */
        public constructor() {
            this.m_count = 0n;
        }

        /**
         * Getter for the current count value.
         *
         * @returns {bigint} The current count.
         */
        public get count(): bigint {
            return this.m_count;
        }

        /**
         * Setter for the count value.
         *
         * @param {bigint} m_count The new value for the count.
         */
        public set count(m_count: bigint) {
            this.m_count = m_count;
            return; // Removed unnecessary return statement
        }

        /**
         * Static factory method that creates a new `CommandCounter` instance
         * and initializes it with the provided count value.
         *
         * @param {bigint} m_count The initial count value for the new instance.
         * @returns {CommandCounter} A new `CommandCounter` instance.
         */
        public static instance(m_count: bigint): CommandCounter {
            const destination = new CommandCounter();
            destination.count = m_count;
            return destination;
        }

        /**
         * Increments the internal command count by 1.
         */
        public increase(): void {
            ++this.m_count;
            return;
        }

        /**
         * Decrements the internal command count by 1.
         */
        public decrease(): void {
            --this.m_count;
            return;
        }
    }
}
