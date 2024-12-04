import pandas as pd
from testlink import TestlinkAPIClient

API_URL = "http://172.17.17.8:8085/lib/api/xmlrpc/v1/xmlrpc.php"
KEY = "10b2132073a17c9d4a0bc700dd778f83"
tlc = TestlinkAPIClient(API_URL, KEY)

def read_test_case_excel(file_path):
    # Read the Excel file using pandas
    df = pd.read_excel(file_path, engine='openpyxl')

    # Strip any extra spaces from column names
    df.columns = df.columns.str.strip()

    # Remove rows where all relevant columns are NaN
    df_cleaned = df.dropna(
        subset=['Category', 'Test case ID', 'Description', 'Keywords', 'Steps_actions', 'expected_results', 'execution_type'],
        how='all'
    )

    # Iterate through the cleaned data
    test_case_data = []
    current_test_case = {}

    for index, row in df_cleaned.iterrows():
        # Check if we are starting a new test case
        if pd.notna(row['Test case ID']):
            # If we have a current test case, append it to the list
            if current_test_case:
                test_case_data.append(current_test_case)

            # Start a new test case dictionary
            current_test_case = {
                'Category': row['Category'],
                'Test Case ID': row['Test case ID'],
                'Description': row['Description'],
                'Keywords': row['Keywords'],
                'Test Cases': row['Test cases'],
                'Expected Output': row['Expected Output'],
                'Steps Data': []  # Initialize Steps Data as an empty list
            }

        # Collect the steps for this test case
        if pd.notna(row['Steps_actions']):
            step = {
                'step_number': len(current_test_case['Steps Data']) + 1,  # Sequential step number
                'actions': row['Steps_actions'],
                'expected_results': row['expected_results'],
                'execution_type': row['execution_type']
            }
            current_test_case['Steps Data'].append(step)

    # Append the last test case after the loop ends
    if current_test_case:
        test_case_data.append(current_test_case)

    # Print all test case data and upload to TestLink
    for test_case in test_case_data:
        print(f"Category: {test_case['Category']}")
        print(f"Test Case ID: {test_case['Test Case ID']}")
        print(f"Description: {test_case['Description']}")
        print(f"Keywords: {test_case['Keywords']}")
        print(f"Test Cases: {test_case['Test Cases']}")
        print(f"Expected Output: {test_case['Expected Output']}")
        print("Steps Data (List of Dictionaries):")
        print(test_case['Steps Data'])  # Print the entire list of dictionaries for steps
        print("-" * 40)

        # Now upload the test case to TestLink
        upload_test_case_to_testlink(test_case)


def upload_test_case_to_testlink(test_case):
    # Assuming you already have the suite_id and project_id available
    suite_id = get_or_create_test_suite(test_case['Category'])
    project_id = get_project_id('Your Project Name')  # Replace with your actual project name

    case_name = f"{test_case['Test Case ID']} - {test_case['Description']}"
    summary = test_case['Test Cases']
    expected_results = test_case['Expected Output']
    steps_list = test_case['Steps Data']
    keywords_list = test_case['Keywords'].split(",") if test_case['Keywords'] else []

    try:
        # Create the test case in TestLink
        test_case_response = tlc.createTestCase(
            testcasename=case_name,
            testsuiteid=suite_id,
            testprojectid=project_id,
            authorlogin="admin",  # Change if necessary
            summary=summary,
            steps=steps_list,  # Steps data (list of dictionaries)
            expectedresults=expected_results
        )
        print(f"Test case '{case_name}' created successfully in suite '{test_case['Category']}'.")

        # Fetch the newly created test case ID
        test_case_id = test_case_response[0]['id']
        print("Test case ID:", test_case_id)

        # Add keywords to the test case if available
        if keywords_list:
            response_keywords = tlc.addTestCaseKeywords({test_case_id: keywords_list})
            print("Keywords added:", response_keywords)

    except Exception as e:
        print(f"Error creating test case '{case_name}': {str(e)}")


def get_project_id(project_name):
    # Fetch the project ID using the TestLink API
    projects = tlc.getProjects()
    print(projects)
    for project in projects:
        if project["name"] == "PCI":
            return project["id"]
    raise ValueError(f"Project '{project_name}' not found.")


def get_or_create_test_suite(suite_name):
    # Fetch or create the test suite using the TestLink API
    project_id = get_project_id("USB")  # Replace with your actual project name
    suites = tlc.getFirstLevelTestSuitesForTestProject(project_id)
    for suite in suites:
        if suite["name"] == suite_name:
            return suite["id"]

    suite = tlc.createTestSuite(project_id, suite_name, "Imported test suite")
    return suite[0]["id"]


# File path to your Excel sheet
file_path = 'TC_Excel_sheet/Copy_of_NVMe_Test_cases.xlsx'
read_test_case_excel(file_path)
