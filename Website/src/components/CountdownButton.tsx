import { Button } from "@mui/material";
import { useEffect, useState } from "react";

const CountdownButton = () => {
    const [timeLeft, setTimeLeft] = useState<string>("");

    useEffect(() => {
        const targetDate = new Date("2024-10-26").getTime();

        const exchanger = (time: number): string => {
            return time < 10 ? `0${time}` : `${time}`;
        };

        const updateCountdown = () => {
            const currentDate = new Date().getTime();
            const timeDiff = targetDate - currentDate;

            if (timeDiff > 0) {
                const days = Math.floor(timeDiff / (1000 * 60 * 60 * 24));
                const hours = Math.floor((timeDiff % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
                const minutes = Math.floor((timeDiff % (1000 * 60 * 60)) / (1000 * 60));
                const seconds = Math.floor((timeDiff % (1000 * 60)) / 1000);

                setTimeLeft(`Available in ${exchanger(days)} days ${exchanger(hours)} hours ${minutes} minutes ${seconds} seconds`);
            } else {
                setTimeLeft("Download Available");
                clearInterval(interval);
            }
        };

        const interval = setInterval(updateCountdown, 1000);

        return () => clearInterval(interval);
    }, []);

    return (
        <Button variant="outlined" sx={{ borderRadius: "8px" }} disabled={timeLeft !== "Download Available"}>
            {timeLeft !== "Download Available" ? `${timeLeft}` : "Download"}
        </Button>
    );
};

export default CountdownButton;
