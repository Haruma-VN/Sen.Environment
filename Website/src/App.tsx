import React, { useState } from "react";
import { Container, CssBaseline, useMediaQuery, Box } from "@mui/material";
import { createTheme, ThemeProvider } from "@mui/material/styles";
import MainContent from "./components/MainContent";
import Footer from "./components/Footer";
import Navbar from "./components/Navbar";
import SidebarDrawer from "./components/SidebarDrawer";

const App: React.FC = () => {
    const isLargeScreen = useMediaQuery("(min-width:1024px)");
    const [isDrawerOpen, setIsDrawerOpen] = useState(isLargeScreen);
    const prefersDarkMode = useMediaQuery("(prefers-color-scheme: dark)");
    const isSmallScreen = useMediaQuery("(max-width:600px)");

    const toggleDrawer = () => {
        setIsDrawerOpen(!isDrawerOpen);
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
        <ThemeProvider theme={theme}>
            <React.Fragment>
                <CssBaseline />
                <Box sx={{ display: "flex", flexDirection: "column", minHeight: "100vh", flexGrow: 1 }}>
                    <Box sx={{ display: "flex", flex: 1 }}>
                        {isLargeScreen && <SidebarDrawer isDrawerOpen={isDrawerOpen} toggleDrawer={toggleDrawer} />}
                        <Box sx={{ flexGrow: 1 }}>
                            <Navbar />
                            <Container component="main" sx={{ flexGrow: 1, my: 2 }}>
                                <MainContent />
                            </Container>
                        </Box>
                    </Box>
                    {!isSmallScreen && <Footer />}
                </Box>
            </React.Fragment>
        </ThemeProvider>
    );
};

export default App;
