#!/bin/bash
# Database utility script for BookZone

DB_FILE="data/library.db"

if [ ! -f "$DB_FILE" ]; then
    echo "Error: Database file not found at $DB_FILE"
    exit 1
fi

show_menu() {
    echo ""
    echo "=== BookZone Database Utility ==="
    echo "1. Show all tables"
    echo "2. Show database schema"
    echo "3. View all bookshelves"
    echo "4. View all books"
    echo "5. View all users"
    echo "6. View borrowed books"
    echo "7. View borrow counts (top 10)"
    echo "8. Show database statistics"
    echo "9. Backup database"
    echo "10. Reset database (clear all data)"
    echo "11. Interactive SQL prompt"
    echo "0. Exit"
    echo ""
    echo -n "Enter your choice: "
}

while true; do
    show_menu
    read choice
    
    case $choice in
        1)
            echo ""
            echo "=== Tables ==="
            sqlite3 "$DB_FILE" ".tables"
            ;;
        2)
            echo ""
            echo "=== Database Schema ==="
            sqlite3 "$DB_FILE" ".schema"
            ;;
        3)
            echo ""
            echo "=== Bookshelves ==="
            sqlite3 -header -column "$DB_FILE" "SELECT * FROM bookshelves ORDER BY id;"
            ;;
        4)
            echo ""
            echo "=== Books ==="
            sqlite3 -header -column "$DB_FILE" "SELECT id, shelf_name, title, author FROM books ORDER BY shelf_name, id;"
            ;;
        5)
            echo ""
            echo "=== Users ==="
            sqlite3 -header -column "$DB_FILE" "SELECT username, CASE WHEN is_admin = 1 THEN 'Yes' ELSE 'No' END as is_admin FROM users ORDER BY username;"
            ;;
        6)
            echo ""
            echo "=== Borrowed Books ==="
            sqlite3 -header -column "$DB_FILE" "SELECT bb.username, bb.shelf_name, bb.book_id, b.title FROM borrowed_books bb LEFT JOIN books b ON bb.book_id = b.id AND bb.shelf_name = b.shelf_name ORDER BY username;"
            ;;
        7)
            echo ""
            echo "=== Top 10 Most Borrowed Books ==="
            sqlite3 -header -column "$DB_FILE" "SELECT bc.book_id, b.title, b.author, bc.count FROM borrow_counts bc LEFT JOIN books b ON bc.book_id = b.id ORDER BY bc.count DESC LIMIT 10;"
            ;;
        8)
            echo ""
            echo "=== Database Statistics ==="
            echo -n "Bookshelves: "
            sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM bookshelves;"
            echo -n "Books: "
            sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM books;"
            echo -n "Users: "
            sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM users;"
            echo -n "Admin Users: "
            sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM users WHERE is_admin = 1;"
            echo -n "Borrowed Books: "
            sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM borrowed_books;"
            echo -n "Books with Borrow History: "
            sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM borrow_counts;"
            echo ""
            echo "Database file size: $(du -h "$DB_FILE" | cut -f1)"
            ;;
        9)
            BACKUP_FILE="data/library_backup_$(date +%Y%m%d_%H%M%S).db"
            cp "$DB_FILE" "$BACKUP_FILE"
            echo ""
            echo "✓ Database backed up to: $BACKUP_FILE"
            ;;
        10)
            echo ""
            echo "⚠ WARNING: This will delete ALL data from the database!"
            echo -n "Are you sure? (type 'yes' to confirm): "
            read confirm
            if [ "$confirm" = "yes" ]; then
                sqlite3 "$DB_FILE" "DELETE FROM borrow_counts; DELETE FROM borrowed_books; DELETE FROM books; DELETE FROM users; DELETE FROM bookshelves;"
                echo "✓ Database cleared. Tables structure preserved."
            else
                echo "Operation cancelled."
            fi
            ;;
        11)
            echo ""
            echo "Entering interactive SQLite prompt..."
            echo "Type .exit or press Ctrl+D to return to the menu"
            echo ""
            sqlite3 "$DB_FILE"
            ;;
        0)
            echo ""
            echo "Goodbye!"
            exit 0
            ;;
        *)
            echo ""
            echo "Invalid choice. Please try again."
            ;;
    esac
    
    echo ""
    echo -n "Press Enter to continue..."
    read
done
