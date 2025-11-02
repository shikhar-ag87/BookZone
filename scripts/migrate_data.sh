#!/bin/bash
# Migration script to import data from old text files to SQLite database

DB_FILE="data/library.db"
LIBRARY_FILE="data/library.txt"
USERS_FILE="data/users.txt"

echo "=== BookZone Data Migration Script ==="
echo "This script migrates data from text files to SQLite database"
echo ""

# Check if database exists
if [ ! -f "$DB_FILE" ]; then
    echo "Error: Database file not found at $DB_FILE"
    echo "Please run the BookZone application once to create the database."
    exit 1
fi

# Backup existing database
BACKUP_FILE="data/library_backup_$(date +%Y%m%d_%H%M%S).db"
cp "$DB_FILE" "$BACKUP_FILE"
echo "✓ Created backup: $BACKUP_FILE"

# Function to parse library.txt and import data
migrate_library() {
    if [ ! -f "$LIBRARY_FILE" ]; then
        echo "⚠ Warning: $LIBRARY_FILE not found. Skipping library migration."
        return
    fi

    echo ""
    echo "Migrating library data from $LIBRARY_FILE..."
    
    local current_shelf=""
    local book_count=0
    local shelf_count=0
    
    while IFS= read -r line; do
        # Skip empty lines
        [[ -z "$line" ]] && continue
        
        # Check if it's a bookshelf line
        if [[ $line == "#Bookshelf,"* ]]; then
            current_shelf="${line#\#Bookshelf,}"
            # Insert bookshelf
            sqlite3 "$DB_FILE" "INSERT OR IGNORE INTO bookshelves (name) VALUES ('$current_shelf');"
            ((shelf_count++))
            echo "  ✓ Added bookshelf: $current_shelf"
        else
            # Parse book line: id,title,author,genres
            IFS=',' read -r book_id title author genres <<< "$line"
            
            if [[ -n "$current_shelf" && -n "$book_id" && -n "$title" ]]; then
                # Escape single quotes for SQL
                title="${title//\'/\'\'}"
                author="${author//\'/\'\'}"
                genres="${genres//\'/\'\'}"
                
                sqlite3 "$DB_FILE" "INSERT OR IGNORE INTO books (id, shelf_name, title, author, genres) VALUES ($book_id, '$current_shelf', '$title', '$author', '$genres');"
                ((book_count++))
            fi
        fi
    done < "$LIBRARY_FILE"
    
    echo "  ✓ Migrated $shelf_count bookshelves and $book_count books"
}

# Function to parse users.txt and import data
migrate_users() {
    if [ ! -f "$USERS_FILE" ]; then
        echo "⚠ Warning: $USERS_FILE not found. Skipping users migration."
        return
    fi

    echo ""
    echo "Migrating users data from $USERS_FILE..."
    
    local user_count=0
    local borrow_count=0
    
    # Format: username,password,is_admin|borrowed_books
    # borrowed_books: ShelfName:BookID;ShelfName:BookID;
    while IFS= read -r line; do
        [[ -z "$line" ]] && continue
        
        # Split by first pipe to separate user info from borrowed books
        local user_part="${line%%|*}"
        local borrowed_part="${line#*|}"
        
        # Parse user info: username,password,is_admin
        IFS=',' read -r username password is_admin <<< "$user_part"
        
        if [[ -n "$username" && -n "$password" ]]; then
            # Escape single quotes for SQL
            username="${username//\'/\'\'}"
            password="${password//\'/\'\'}"
            
            sqlite3 "$DB_FILE" "INSERT OR IGNORE INTO users (username, password, is_admin) VALUES ('$username', '$password', $is_admin);"
            ((user_count++))
            echo "  ✓ Added user: $username (admin=$is_admin)"
            
            # Parse borrowed books if they exist
            if [[ "$borrowed_part" != "$user_part" && -n "$borrowed_part" ]]; then
                # Remove trailing semicolon and split by semicolon
                borrowed_part="${borrowed_part%;}"
                IFS=';' read -ra BOOKS <<< "$borrowed_part"
                
                for book_entry in "${BOOKS[@]}"; do
                    if [[ -n "$book_entry" ]]; then
                        IFS=':' read -r shelf_name book_id <<< "$book_entry"
                        shelf_name="${shelf_name//\'/\'\'}"
                        
                        sqlite3 "$DB_FILE" "INSERT OR IGNORE INTO borrowed_books (username, shelf_name, book_id) VALUES ('$username', '$shelf_name', $book_id);"
                        sqlite3 "$DB_FILE" "INSERT INTO borrow_counts (book_id, count) VALUES ($book_id, 1) ON CONFLICT(book_id) DO UPDATE SET count = count + 1;"
                        ((borrow_count++))
                    fi
                done
            fi
        fi
    done < "$USERS_FILE"
    
    echo "  ✓ Migrated $user_count users with $borrow_count borrowed books"
}

# Show current database stats
show_stats() {
    echo ""
    echo "=== Current Database Statistics ==="
    echo -n "Bookshelves: "
    sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM bookshelves;"
    echo -n "Books: "
    sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM books;"
    echo -n "Users: "
    sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM users;"
    echo -n "Borrowed Books: "
    sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM borrowed_books;"
    echo -n "Borrow Count Records: "
    sqlite3 "$DB_FILE" "SELECT COUNT(*) FROM borrow_counts;"
}

# Perform migration
migrate_library
migrate_users
show_stats

echo ""
echo "=== Migration Complete ==="
echo ""
echo "Next steps:"
echo "1. The old text files are preserved in the data/ directory"
echo "2. You can safely delete them after verifying the database"
echo "3. Run the application to add data through the UI if needed"
echo "4. Database file: $DB_FILE"
echo ""
