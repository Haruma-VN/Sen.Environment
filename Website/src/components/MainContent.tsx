import React from "react";
import { Card, CardMedia, CardContent, Typography, Button, Box, useMediaQuery } from "@mui/material";
import { createTheme, ThemeProvider } from "@mui/material/styles";
import senLogo from "../assets/logo.ico";

const MainContent: React.FC = () => {
    const prefersDarkMode = useMediaQuery("(prefers-color-scheme: dark)");
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
        <ThemeProvider theme={theme}>
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
                <CardContent>
                    <Typography variant="h5" component="div">
                        Sen
                    </Typography>
                    <Typography variant="body2" color="text.secondary">
                        Sen is a PvZ2-Modding tool created by Haruma & is the recommended tool by most modders.
                    </Typography>
                    <Box sx={{ position: "absolute", bottom: "10px", right: "10px" }}>
                        <Button variant="outlined" sx={{ borderRadius: "8px" }}>
                            Download
                        </Button>
                    </Box>
                </CardContent>
            </Card>
        </ThemeProvider>
    );
};

export default MainContent;
