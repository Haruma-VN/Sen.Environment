import React, { useState } from "react";
import { AppBar, Toolbar, Typography, IconButton, Dialog, DialogActions, DialogContent, DialogContentText, DialogTitle, Button, useMediaQuery } from "@mui/material";
import InfoIcon from "@mui/icons-material/Info";
import { createTheme, ThemeProvider } from "@mui/material/styles";

const Navbar: React.FC = () => {
    const [open, setOpen] = useState(false);

    // Detect the system's color scheme preference
    const prefersDarkMode = useMediaQuery("(prefers-color-scheme: dark)");

    // Create a theme that switches between dark and light mode
    const theme = React.useMemo(
        () =>
            createTheme({
                palette: {
                    mode: prefersDarkMode ? "dark" : "light",
                },
            }),
        [prefersDarkMode],
    );

    const handleInfoClick = () => {
        setOpen(true);
    };

    const handleClose = () => {
        setOpen(false);
    };

    return (
        <ThemeProvider theme={theme}>
            <React.Fragment>
                <AppBar position="static">
                    <Toolbar>
                        <Typography variant="h6" component="div" sx={{ flexGrow: 1 }}>
                            Sen.Environment
                        </Typography>
                        <IconButton color="inherit" onClick={handleInfoClick}>
                            <InfoIcon />
                        </IconButton>
                    </Toolbar>
                </AppBar>

                <Dialog open={open} onClose={handleClose}>
                    <DialogTitle>About</DialogTitle>
                    <DialogContent>
                        <DialogContentText>Sen.Environment is an open-source project that helps you modify the game files.</DialogContentText>
                    </DialogContent>
                    <DialogActions>
                        <Button variant="outlined" sx={{ borderRadius: "8px" }} autoFocus onClick={() => (window.location.href = "https://github.com/Haruma-VN/Sen.Environment")}>
                            GitHub repo
                        </Button>
                        <Button variant="outlined" sx={{ borderRadius: "8px" }} onClick={handleClose} color="primary">
                            Close
                        </Button>
                    </DialogActions>
                </Dialog>
            </React.Fragment>
        </ThemeProvider>
    );
};

export default Navbar;
