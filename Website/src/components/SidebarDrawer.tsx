import React, { useState } from "react";
import { Drawer, List, ListItem, ListItemIcon, ListItemText, ListItemButton, useMediaQuery, Box, IconButton } from "@mui/material";
import HomeIcon from "@mui/icons-material/Home";
import InfoIcon from "@mui/icons-material/Info";
import AssignmentIcon from "@mui/icons-material/Assignment";
import MenuIcon from "@mui/icons-material/Menu";

const SidebarDrawer: React.FC<{ isDrawerOpen: boolean; toggleDrawer: () => void }> = ({ isDrawerOpen, toggleDrawer }) => {
    const isLargeScreen = useMediaQuery("(min-width:1024px)");
    const [isCollapsed, setIsCollapsed] = useState(true);

    const toggleCollapse = () => {
        setIsCollapsed(!isCollapsed);
    };

    const drawerWidth = isCollapsed ? 60 : 240;

    return (
        <Drawer
            variant={isLargeScreen ? "persistent" : "temporary"}
            open={isDrawerOpen}
            onClose={toggleDrawer}
            sx={{
                width: drawerWidth,
                flexShrink: 0,
                [`& .MuiDrawer-paper`]: {
                    width: drawerWidth,
                    boxSizing: "border-box",
                    overflow: "hidden",
                },
            }}
        >
            <Box sx={{ display: "flex", justifyContent: "flex-end", p: 1, marginTop: 1 }}>
                <IconButton onClick={toggleCollapse} color="inherit">
                    <MenuIcon />
                </IconButton>
            </Box>
            <Box sx={{ width: drawerWidth }}>
                <List>
                    <ListItem key="Home" disablePadding>
                        <ListItemButton>
                            <ListItemIcon>
                                <HomeIcon />
                            </ListItemIcon>
                            {!isCollapsed && <ListItemText primary="Home" />}
                        </ListItemButton>
                    </ListItem>
                    <ListItem key="Documentation" disablePadding>
                        <ListItemButton>
                            <ListItemIcon>
                                <AssignmentIcon />
                            </ListItemIcon>
                            {!isCollapsed && <ListItemText primary="Documentation" />}
                        </ListItemButton>
                    </ListItem>
                    <ListItem key="About" disablePadding>
                        <ListItemButton>
                            <ListItemIcon>
                                <InfoIcon />
                            </ListItemIcon>
                            {!isCollapsed && <ListItemText primary="About" />}
                        </ListItemButton>
                    </ListItem>
                </List>
            </Box>
        </Drawer>
    );
};

export default SidebarDrawer;
