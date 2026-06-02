<?php
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $servername = "localhost";
    $username = "root";
    $password = "";
    $dbname = "lost_and_found";

    // Create connection
    $conn = new mysqli($servername, $username, $password, $dbname);

    // Check connection
    if ($conn->connect_error) {
        die("Connection failed: " . $conn->connect_error);
    }

    // Retrieve reference email
    $refEmail = isset($_POST['ref_email']) ? $_POST['ref_email'] : null; // Reference Email

    // Check if ref_email is set, otherwise stop
    if (!$refEmail) {
        die("Missing reference email.");
    }

    // Handle Lost Items
    if (isset($_POST['lost-item'])) {
        $item = isset($_POST['lost-item']) ? $_POST['lost-item'] : null;
        $location = isset($_POST['lost-location']) ? $_POST['lost-location'] : null;
        $date = isset($_POST['lost-date']) ? $_POST['lost-date'] : null;

        // Ensure all lost item fields are filled
        if ($item && $location && $date) {
            // Prepare and execute SQL for lost items
            $sql = "INSERT INTO lost_items (item_description, location_lost, date_lost, ref_email) VALUES (?, ?, ?, ?)";
            $stmt = $conn->prepare($sql);
            $stmt->bind_param("ssss", $item, $location, $date, $refEmail);

            if ($stmt->execute()) {
                // Redirect to found_items.php on success
                header("Location: found_items.php");
                exit; // Ensure the script stops after redirection
            } else {
                echo "Error: " . $stmt->error;
            }
            $stmt->close();
        } else {
            echo "Please provide all required fields for the lost item.";
        }

    // Handle Found Items
    } elseif (isset($_POST['found-item'])) {
        $item = isset($_POST['found-item']) ? $_POST['found-item'] : null;
        $location = isset($_POST['found-location']) ? $_POST['found-location'] : null;
        $date = isset($_POST['found-date']) ? $_POST['found-date'] : null;
        $imagePath = null;

        // Ensure all found item fields are filled
        if ($item && $location && $date) {
            // Handle image upload if provided
            if (isset($_FILES['found-image']) && $_FILES['found-image']['error'] == 0) {
                $targetDir = "uploads/";
                // Ensure upload directory exists
                if (!is_dir($targetDir)) {
                    mkdir($targetDir, 0777, true);
                }
                $targetFile = $targetDir . basename($_FILES["found-image"]["name"]);

                // Validate file type and size
                $allowedTypes = ['image/jpeg', 'image/png', 'image/gif'];
                $maxFileSize = 2 * 1024 * 1024; // 2MB

                if (in_array($_FILES["found-image"]["type"], $allowedTypes) && $_FILES["found-image"]["size"] <= $maxFileSize) {
                    if (move_uploaded_file($_FILES["found-image"]["tmp_name"], $targetFile)) {
                        $imagePath = $targetFile;
                    } else {
                        die("Error: Failed to move uploaded file.");
                    }
                } else {
                    die("Error: Invalid file type or file size exceeds 2MB.");
                }
            }

            // Prepare and execute SQL for found items
            $sql = "INSERT INTO found_items (item_description, location_found, date_found, ref_email, image_path) VALUES (?, ?, ?, ?, ?)";
            $stmt = $conn->prepare($sql);
            $stmt->bind_param("sssss", $item, $location, $date, $refEmail, $imagePath);

            if ($stmt->execute()) {
                // Redirect to found_items.php on success
                header("Location: found_items.php");
                exit; // Ensure the script stops after redirection
            } else {
                echo "Error: " . $stmt->error;
            }
            $stmt->close();
        } else {
            echo "Please provide all required fields for the found item.";
        }
    }

    $conn->close();
} else {
    echo "Invalid request method!";
}
?>
