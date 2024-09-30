import React, { useState } from "react";
import { AppBar, Toolbar, Typography, Dialog, DialogActions, DialogContent, DialogContentText, DialogTitle, Button, useMediaQuery } from "@mui/material";
import { createTheme, ThemeProvider } from "@mui/material/styles";

const Navbar: React.FC = () => {
    const [open, setOpen] = useState(false);
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

    const handleClose = () => {
        setOpen(false);
    };

    return (
        <ThemeProvider theme={theme}>
            <React.Fragment>
                <AppBar position="static">
                    <Toolbar>
                        <Typography variant="h6" component="div" sx={{ flexGrow: 1, textAlign: "left" }}>
                            Sen.Environment
                        </Typography>
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
