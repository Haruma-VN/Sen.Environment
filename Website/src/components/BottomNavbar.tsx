import { useState } from "react";
import { BottomNavigation, BottomNavigationAction, Paper, useMediaQuery } from "@mui/material";
import HomeIcon from "@mui/icons-material/Home";
import InfoIcon from "@mui/icons-material/Info";
import AssignmentIcon from "@mui/icons-material/Assignment";

const BottomNavBar = () => {
    const [value, setValue] = useState(0);
    const isSmallScreen = useMediaQuery("(max-width:1024px)");

    return isSmallScreen ? (
        <Paper sx={{ position: "fixed", bottom: 0, left: 0, right: 0 }} elevation={3}>
            <BottomNavigation showLabels value={value} onChange={(_, newValue) => setValue(newValue)}>
                <BottomNavigationAction label="Home" icon={<HomeIcon />} />
                <BottomNavigationAction label="Documentation" icon={<AssignmentIcon />} />
                <BottomNavigationAction label="About" icon={<InfoIcon />} />
            </BottomNavigation>
        </Paper>
    ) : null;
};

export default BottomNavBar;
