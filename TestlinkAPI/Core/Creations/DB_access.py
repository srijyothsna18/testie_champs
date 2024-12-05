import psycopg2
from Utils.Inputs.Common_inputs import DB

def get_ids_and_srs_ids_by_req_doc_names(req_doc_names):
    # Establish the connection to the database
    connection = psycopg2.connect(
        host=DB.host,
        database=DB.database,
        user=DB.user,
        password=DB.password
    )

    cursor = connection.cursor()

    query = "SELECT id, srs_id FROM requirements WHERE req_doc_id IN %s;"

    cursor.execute(query, (tuple(req_doc_names),))
    results = cursor.fetchall()

    cursor.close()
    connection.close()

    return results


# Example usage
if __name__ == "__main__":
    print("Enter SRS names separated by commas (e.g., use-case-01,feature-01):")
    input_srs_names = input().strip()

    srs_names = [name.strip() for name in input_srs_names.split(",")]

    results = get_ids_and_srs_ids_by_req_doc_names(srs_names)

    if results:
        print("Fetched IDs and SRS IDs:")
        for req_id, srs_id in results:
            print(f"Requirement ID: {req_id}, SRS ID: {srs_id}")
    else:
        print("No matching records found.")