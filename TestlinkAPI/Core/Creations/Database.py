import psycopg2


def fetch_srs_ids():
    try:
        # Connect to the PostgreSQL database
        connection = psycopg2.connect(
            dbname="testlink-1.9.19",
            user="tlink",
            password="tlink",
            host = "tl_pg",
            port= 8086
        )
        cursor = connection.cursor()

        # Query to fetch srs_id from req_specs table
        query = "SELECT srs_id FROM req_specs;"
        cursor.execute(query)

        # Fetch all results
        srs_ids = cursor.fetchall()

        # Print results
        print("srs_id from req_specs table:")
        for srs_id in srs_ids:
            print(srs_id[0])  # srs_id is a tuple, so we print the first element

    except Exception as e:
        print("Error occurred:", e)

    finally:
        # Close the database connection
        if connection:
            cursor.close()
            connection.close()


# Call the function
fetch_srs_ids()
