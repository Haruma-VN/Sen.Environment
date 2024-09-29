import React from "react";
import { Box, Typography } from "@mui/material";

const Footer: React.FC = () => {
    return (
        <Box
            component="footer"
            sx={{
                textAlign: "center",
                p: 2,
                mt: "auto",
                bgcolor: "background.paper",
                width: "100%",
            }}
        >
            <Typography variant="body2" color="text.secondary">
                &copy; 2024 Copyright Haruma. All rights reserved.
            </Typography>
        </Box>
    );
};

export default Footer;
