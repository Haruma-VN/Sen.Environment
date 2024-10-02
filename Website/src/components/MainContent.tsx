import React, { useState, useEffect } from "react";
import { Card, CardMedia, CardContent, Typography, Button, Box, useMediaQuery, Snackbar } from "@mui/material";
import { createTheme, ThemeProvider } from "@mui/material/styles";
import senLogo from "../assets/logo.ico";
import detectOS from "../utility/Platform";
import { GitHubAPI } from "../models/GitHub";
import DownloadButton from "./DownloadButton";
import BottomNavBar from "./BottomNavbar";

const MainContent: React.FC<{ setActivePage: (value: string) => void }> = ({ setActivePage }) => {
    const prefersDarkMode = useMediaQuery("(prefers-color-scheme: dark)");
    const allowance: Array<string> = ["windows", "android", "linux", "macintosh", "ios"];
    const [message, setMessage] = useState<string>("");
    const [open, setOpen] = useState<boolean>(false);
    const includeOS = allowance.includes(detectOS());
    const [downloadCount, setDownloadCount] = useState<number>(0);

    useEffect(() => {
        const link: string = "https://api.github.com/repos/Haruma-VN/Sen.Environment/releases/tags/release";
        const fetchData = async () => {
            const response = await fetch(link, {
                method: "GET",
                headers: { "Content-Type": "application/json" },
            });
            if (!response.ok) {
                throw new Error("An error found when fetching url, retry later");
            }
            return (await response.json()) as GitHubAPI;
        };
        fetchData()
            .then((data) => setDownloadCount(data.assets.reduce((a, b) => a + b.download_count, 0)))
            .catch((e) => {
                setMessage(e.message);
                handleOpenDialog();
            });
    }, []);

    const handleOpenDialog = () => {
        setOpen(true);
    };

    const handleClose = (_?: React.SyntheticEvent | Event, reason?: string) => {
        if (reason === "clickaway") {
            return;
        }
        setOpen(false);
    };

    const handleDownload = () => {
        const link: string = "https://api.github.com/repos/Haruma-VN/Sen.Environment/releases/tags/release";
        const download = async () => {
            const response = await fetch(link, {
                method: "GET",
                headers: { "Content-Type": "application/json" },
            });
            if (!response.ok) {
                throw new Error("An error found when fetching url, retry later");
            }
            const data = (await response.json()) as GitHubAPI;
            let baseUrl = "";
            switch (detectOS()) {
                case "windows":
                    baseUrl = data.assets.find((e) => e.name === "win-x64.zip")!.browser_download_url;
                    break;
                case "android":
                    break;
                default:
                    throw new Error("Current operating system does not supported");
            }
            window.location.href = baseUrl;
            setDownloadCount(downloadCount + 1);
        };
        download()
            .then(() => setMessage("Download success, enjoy the tool!"))
            .catch((e) => setMessage(e.message))
            .finally(() => handleOpenDialog());
    };

    const theme = React.useMemo(
        () =>
            createTheme({
                palette: {
                    mode: prefersDarkMode ? "dark" : "light",
                },
            }),
        [prefersDarkMode],
    );
    return (
        <React.Fragment>
            <ThemeProvider theme={theme}>
                <Box sx={{ display: "flex" }}>
                    <Card
                        sx={{
                            position: "relative",
                            paddingBottom: "20px",
                            boxShadow: 3,
                            transition: "box-shadow 0.3s",
                            "&:hover": { boxShadow: 6 },
                        }}
                    >
                        <CardMedia
                            component="img"
                            image={senLogo}
                            alt="Product Image"
                            sx={{
                                height: { xs: 300, md: 400 },
                                width: "100%",
                                objectFit: "cover",
                            }}
                        />
                        <CardContent sx={{ padding: 3, position: "relative" }}>
                            <Typography variant="h5" component="div" sx={{ fontWeight: "bold", mb: 2 }}>
                                Sen
                            </Typography>

                            <Typography variant="body1" color="text.secondary" sx={{ mb: 3 }}>
                                Sen is a PvZ2-Modding tool created by Haruma & is the recommended tool by most modders.
                            </Typography>

                            <Box sx={{ mb: 3 }}>
                                <Typography variant="body2" color="text.secondary">
                                    Your operating system is <strong>{detectOS()}</strong> which is <strong>{includeOS ? "supported for download" : "unsupported at the moment"}</strong>.
                                </Typography>
                                <Typography variant="body2" color="text.secondary" sx={{ mt: 1 }}>
                                    Download count: <strong>{downloadCount}</strong>
                                </Typography>
                            </Box>

                            <Box sx={{ position: "absolute", bottom: 2, right: 16 }}>
                                {includeOS ? (
                                    <DownloadButton handleDownload={handleDownload} />
                                ) : (
                                    <Button variant="outlined" sx={{ borderRadius: "8px" }} disabled>
                                        Unsupported
                                    </Button>
                                )}
                            </Box>
                        </CardContent>
                    </Card>
                    <Snackbar
                        open={open}
                        autoHideDuration={6000}
                        onClose={handleClose}
                        message={message}
                        action={
                            <Button color="secondary" size="small" onClick={handleClose}>
                                Close
                            </Button>
                        }
                    />
                    <BottomNavBar setActivePage={setActivePage} />
                </Box>
            </ThemeProvider>
        </React.Fragment>
    );
};

export default MainContent;
