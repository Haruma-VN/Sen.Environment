import { useState, useEffect } from "react";
import { Button } from "@mui/material";
import CountdownButton from "./CountdownButton";

const DownloadButton: React.FC<{ handleDownload: () => void }> = ({ handleDownload }) => {
    const [isDownloadAvailable, setIsDownloadAvailable] = useState(false);

    useEffect(() => {
        const targetDate = new Date("2024-10-26");
        const currentDate = new Date();
        setIsDownloadAvailable(currentDate >= targetDate);
    }, []);

    return isDownloadAvailable ? (
        <Button onClick={handleDownload} variant="contained" color="primary" sx={{ borderRadius: "8px", marginTop: 10 }}>
            Download
        </Button>
    ) : (
        <CountdownButton />
    );
};

export default DownloadButton;
