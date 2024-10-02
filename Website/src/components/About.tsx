import { Container, Box, Typography, Link, List, ListItem, ListItemText, Divider } from "@mui/material";
import GitHubIcon from "@mui/icons-material/GitHub";
import MailOutlineIcon from "@mui/icons-material/MailOutline";
import senLogo from "../assets/sen.png";

const About = () => {
    return (
        <Container>
            <Box
                sx={{
                    display: "flex",
                    alignItems: "center",
                    justifyContent: "center",
                    flexDirection: "column",
                    padding: "20px 0",
                    bgcolor: "#333",
                    color: "white",
                }}
            >
                <img src={senLogo} alt="Logo" style={{ maxWidth: "150px", height: "auto" }} />
                <Typography variant="h4" component="h1" sx={{ mt: 2 }}>
                    Sen.Environment
                </Typography>
            </Box>
            <Box sx={{ mt: 4 }}>
                <Typography variant="h5" gutterBottom>
                    About
                </Typography>
                <Typography variant="body1" gutterBottom>
                    Sen.Environment is a cross-platform project that summarizes my personal knowledge in C++ & TypeScript programming.
                </Typography>
            </Box>
            <Box sx={{ mt: 4 }}>
                <Typography variant="h5" gutterBottom>
                    Open Source Project
                </Typography>
                <Typography variant="body1" gutterBottom>
                    This project is licensed under the <strong>GPL-3.0 License</strong>. You are free to use and modify the code, as long as you follow the license's terms.
                </Typography>
            </Box>
            <Box sx={{ mt: 4 }}>
                <Typography variant="h5" gutterBottom>
                    Platform Support
                </Typography>
                <List>
                    <ListItem>
                        <ListItemText primary="Windows x64+" />
                    </ListItem>
                    <ListItem>
                        <ListItemText primary="Linux x64+" />
                    </ListItem>
                    <ListItem>
                        <ListItemText primary="Macintosh x64+" />
                    </ListItem>
                    <ListItem>
                        <ListItemText primary="iPhone iOS 10.0+" />
                    </ListItem>
                    <ListItem>
                        <ListItemText primary="Android 7+" />
                    </ListItem>
                </List>
            </Box>
            <Box sx={{ mt: 4 }}>
                <Typography variant="h5" gutterBottom>
                    Modules
                </Typography>
                <List>
                    <ListItem>
                        <ListItemText primary="Kernel - Backend, the core of the program." />
                    </ListItem>
                    <ListItem>
                        <ListItemText primary="Shell - Frontend, provides a command-line interface." />
                    </ListItem>
                    <ListItem>
                        <ListItemText primary="Script - Script control flows of the tool." />
                    </ListItem>
                </List>
            </Box>
            <Box sx={{ mt: 4 }}>
                <Typography variant="h5" gutterBottom>
                    Technology
                </Typography>
                <List>
                    <ListItem>
                        <ListItemText primary="CMake" secondary="Used with both Kernel and Shell to support cross-platform with C++." />
                    </ListItem>
                    <ListItem>
                        <ListItemText primary="Flutter" secondary="Helps build beautiful applications, used with WinShell." />
                    </ListItem>
                </List>
            </Box>
            <Box sx={{ mt: 4 }}>
                <Typography variant="h5" gutterBottom>
                    Acknowledgement
                </Typography>
                <Typography variant="body1" gutterBottom>
                    Special thanks to{" "}
                    <Link href="https://github.com/twinkles-twinstar/" target="_blank" rel="noopener">
                        TwinStar
                    </Link>{" "}
                    for his project{" "}
                    <Link href="https://github.com/twinkles-twinstar/TwinStar.ToolKit" target="_blank" rel="noopener">
                        TwinStar.ToolKit
                    </Link>
                    , which has helped me a lot in making this project.
                </Typography>
            </Box>
            <Divider sx={{ my: 4 }} />
            <Box sx={{ display: "flex", justifyContent: "space-between", alignItems: "center" }}>
                <Typography variant="body1">
                    Issues or questions? Reach me at:{" "}
                    <Link href="mailto:harumatsx@gmail.com" target="_blank" rel="noopener">
                        harumatsx@gmail.com
                    </Link>
                </Typography>
                <Box sx={{ display: "flex", gap: 2 }}>
                    <Link href="https://github.com/Haruma-VN/Sen.Environment" target="_blank" rel="noopener">
                        <GitHubIcon />
                    </Link>
                    <Link href="mailto:harumatsx@gmail.com" target="_blank" rel="noopener">
                        <MailOutlineIcon />
                    </Link>
                </Box>
            </Box>
        </Container>
    );
};

export default About;
