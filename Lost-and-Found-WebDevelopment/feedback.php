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

    // Retrieve data from form
    $id = $_POST['id'];
    $type = $_POST['type']; // "lost" or "found"
    $feedback = $_POST['feedback'];

    // Delete the item from the appropriate table
    if ($type === "lost") {
        $sql = "DELETE FROM lost_items WHERE id = ?";
    } else {
        $sql = "DELETE FROM found_items WHERE id = ?";
    }

    $stmt = $conn->prepare($sql);
    $stmt->bind_param("i", $id);

    if ($stmt->execute()) {
        // Log the feedback (optional)
        $log_sql = "INSERT INTO feedback (item_id, type, feedback_text) VALUES (?, ?, ?)";
        $log_stmt = $conn->prepare($log_sql);
        $log_stmt->bind_param("iss", $id, $type, $feedback);
        $log_stmt->execute();
        $log_stmt->close();

        // Redirect to found_items.php
        header("Location: found_items.php");
        exit;
    } else {
        echo "Error: " . $stmt->error;
    }

    $stmt->close();
    $conn->close();
} else {
    // Show the feedback form
    $id = $_GET['id'];
    $type = $_GET['type']; // "lost" or "found"
    ?>
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>Submit Feedback</title>
        <link href="https://fonts.googleapis.com/css2?family=Roboto:wght@400;500;700&display=swap" rel="stylesheet">
        <style>
            /* Global Styles */
            body {
                font-family: 'Roboto', sans-serif;
                margin: 0;
                padding: 0;
                background: linear-gradient(120deg, #74ebd5, #9face6);
                display: flex;
                justify-content: center;
                align-items: center;
                height: 100vh;
                color: #333;
            }

            .container {
                background: #fff;
                padding: 20px;
                border-radius: 10px;
                box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
                width: 100%;
                max-width: 400px;
                text-align: center;
                transition: transform 0.3s ease;
            }

            .container:hover {
                transform: translateY(-5px);
            }

            h1 {
                font-size: 1.8em;
                margin-bottom: 20px;
                color: #4a4a4a;
            }

            label {
                display: block;
                font-weight: bold;
                margin: 10px 0 5px;
                text-align: left;
            }

            textarea {
                width: 100%;
                padding: 10px;
                border: 1px solid #ddd;
                border-radius: 5px;
                resize: none;
                font-size: 1em;
                transition: border-color 0.3s ease;
            }

            textarea:focus {
                border-color: #3498db;
                outline: none;
            }

            button {
                margin-top: 15px;
                padding: 10px 20px;
                font-size: 1em;
                color: #fff;
                background: #3498db;
                border: none;
                border-radius: 5px;
                cursor: pointer;
                transition: background-color 0.3s ease, transform 0.2s ease;
            }

            button:hover {
                background-color: #2980b9;
                transform: scale(1.05);
            }

            button:active {
                transform: scale(0.95);
            }

            .back-link {
                display: inline-block;
                margin-top: 15px;
                color: #3498db;
                text-decoration: none;
                font-weight: bold;
            }

            .back-link:hover {
                text-decoration: underline;
            }

            @media (max-width: 480px) {
                h1 {
                    font-size: 1.5em;
                }
            }
        </style>
    </head>
    <body>
        <div class="container">
            <h1>Submit Your Feedback</h1>
            <form method="POST" action="feedback.php" onsubmit="return confirm('Are you sure you want to submit feedback and delete this item?');">
                <input type="hidden" name="id" value="<?php echo htmlspecialchars($id); ?>">
                <input type="hidden" name="type" value="<?php echo htmlspecialchars($type); ?>">
                <label for="feedback">Your Feedback:</label>
                <textarea name="feedback" id="feedback" rows="5" required placeholder="Write your feedback here..."></textarea>
                <button type="submit">Submit Feedback</button>
            </form>
            <a href="found_items.php" class="back-link">← Go Back to Items</a>
        </div>
    </body>
    </html>
    <?php
}
?>
