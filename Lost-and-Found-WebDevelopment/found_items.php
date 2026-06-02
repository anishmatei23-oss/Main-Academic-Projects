<?php
// Database connection
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "lost_and_found";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);

// Check the connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Fetch Lost Items
$sql_lost = "SELECT * FROM lost_items ORDER BY date_lost DESC";
$result_lost = $conn->query($sql_lost);

// Fetch Found Items
$sql_found = "SELECT * FROM found_items ORDER BY date_found DESC";
$result_found = $conn->query($sql_found);
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Lost and Found Items</title>
    <link rel="stylesheet" href="frontcss.css">
    <style>
        /* General Styles */
        body {
            font-family: 'Roboto', sans-serif;
            background: linear-gradient(to right, #f0f4f8, #dfe7f1);
            margin: 0;
            padding: 0;
            opacity: 0;
            transition: opacity 1s ease-in-out;
        }

        body.loaded {
            opacity: 1;
        }

       /* Header Styles */
header {
    background: linear-gradient(135deg, #50c878, #4e9f8f); /* Peacock-like gradient colors */
    color: #fff;
    padding: 40px 20px;
    text-align: center;
    position: relative;
    box-shadow: 0 4px 15px rgba(0, 0, 0, 0.1);
}

header h1 {
    margin: 0;
    font-size: 3em;
    font-weight: bold;
    letter-spacing: 2px;
    color: #fff; /* White text for better contrast */
}

header p {
    margin-top: 10px;
    font-size: 1.3em;
    color: #f8f9fa; /* Light color for the subtitle */
}

.home-link {
    position: absolute;
    top: 15px;
    left: 20px;
    font-size: 1.2em;
    color: #fff;
    text-decoration: none;
    background: #4caf50;
    padding: 10px 20px;
    border-radius: 5px;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.2);
    transition: background 0.3s ease, transform 0.3s ease;
}

        .home-link:hover {
            background: #45a049;
            transform: scale(1.05);
            box-shadow: 0 6px 8px rgba(0, 0, 0, 0.3);
        }

        /* Flexbox Container for Split Layout */
        .main-content {
            display: flex;
            justify-content: space-between;
            gap: 20px;
            padding: 30px;
        }

        /* Section Styling */
        .section {
            width: 48%; /* Each section takes up 48% of the width */
            padding: 20px;
            background: #fff;
            border-radius: 10px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            overflow-y: auto; /* Allows scrolling if content overflows */
        }

        .section h2 {
            font-size: 1.8em;
            margin-bottom: 15px;
            text-align: center;
            color: #2c3e50;
        }

        /* Table Styles */
        table {
            width: 100%;
            border-collapse: collapse;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
            border-radius: 10px;
            background: #ffffff;
            margin-bottom: 30px;
        }

        th, td {
            padding: 12px 20px;
            text-align: left;
        }

        th {
            background: linear-gradient(135deg, #8e44ad, #3498db);
            color: white;
            font-size: 1.1em;
        }

        td {
            background: #f4f7fa;
            font-size: 1em;
        }

        tr:nth-child(even) td {
            background: #e1e9f0;
        }

        tr:hover td {
            background-color: #f1c40f;
            color: white;
        }

        .item-image {
            width: 100px;
            height: 100px;
            object-fit: cover;
            border-radius: 8px;
            transition: transform 0.3s ease;
        }

        .item-image:hover {
            transform: scale(1.1);
        }

        /* Footer Styles */
        footer {
            background-color: #333;
            color: white;
            padding: 20px;
            text-align: center;
            margin-top: 30px;
        }

        footer p {
            margin: 0;
        }

        /* Media Queries */
        @media (max-width: 768px) {
            header h1 {
                font-size: 2.5em;
            }

            .main-content {
                padding: 20px;
                flex-direction: column;
                align-items: center;
            }

            .section {
                width: 100%;
                margin-bottom: 20px;
            }

            table {
                width: 100%;
            }
        }

    </style>
</head>
<body>
    <header>
        <a href="home.html" class="home-link">Home</a>
        <h1>Lost and Found</h1>
        <p>Your belongings are just a click away</p>
    </header>

    <div class="main-content">
        <!-- Left Half - Lost Items Section -->
        <div class="section">
            <h2>Lost Items</h2>
            <?php if ($result_lost->num_rows > 0): ?>
                <table>
                    <thead>
                        <tr>
                            <th>Item Description</th>
                            <th>Location Lost</th>
                            <th>Date Lost</th>
                            <th>Contact</th>
                        </tr>
                    </thead>
                    <tbody>
    <?php while ($row = $result_lost->fetch_assoc()): ?>
        <tr>
            <td><?php echo htmlspecialchars($row['item_description']); ?></td>
            <td><?php echo htmlspecialchars($row['location_lost']); ?></td>
            <td><?php echo htmlspecialchars($row['date_lost']); ?></td>
            <td><?php echo htmlspecialchars($row['ref_email']); ?></td>
            <td>
                <a href="feedback.php?id=<?php echo $row['id']; ?>&type=lost" class="feedback-link">Click Here</a>
            </td>
        </tr>
    <?php endwhile; ?>
</tbody>

                </table>
            <?php else: ?>
                <p>No lost items to display.</p>
            <?php endif; ?>
        </div>

        <!-- Right Half - Found Items Section -->
        <div class="section">
            <h2>Found Items</h2>
            <?php if ($result_found->num_rows > 0): ?>
                <table>
                    <thead>
                        <tr>
                            <th>Item Description</th>
                            <th>Location Found</th>
                            <th>Date Found</th>
                            <th>Contact Details</th>
                            <th>Image</th>
                        </tr>
                    </thead>
                    <tbody>
    <?php while ($row = $result_found->fetch_assoc()): ?>
        <tr>
            <td><?php echo htmlspecialchars($row['item_description']); ?></td>
            <td><?php echo htmlspecialchars($row['location_found']); ?></td>
            <td><?php echo htmlspecialchars($row['date_found']); ?></td>
            <td><?php echo htmlspecialchars($row['ref_email']); ?></td>
            <td>
                <?php if (!empty($row['image_path'])): ?>
                    <img src="<?php echo htmlspecialchars($row['image_path']); ?>" alt="Found Item" class="item-image">
                <?php else: ?>
                    <p>No image</p>
                <?php endif; ?>
            </td>
            <td>
                <a href="feedback.php?id=<?php echo $row['id']; ?>&type=found" class="feedback-link">Click Here</a>
            </td>
        </tr>
    <?php endwhile; ?>
</tbody>

                </table>
            <?php else: ?>
                <p>No found items to display.</p>
            <?php endif; ?>
        </div>
    </div>

    <footer>
        <p>&copy; 2024 Lost and Found. All rights reserved.</p>
    </footer>

    <?php
    // Close the database connection
    $conn->close();
    ?>

    <script>
        document.addEventListener("DOMContentLoaded", function () {
            document.body.classList.add('loaded');
        });
    </script>
</body>
</html>
