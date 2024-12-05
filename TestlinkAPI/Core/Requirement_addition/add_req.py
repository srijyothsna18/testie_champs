import psycopg2
from testlink import TestlinkAPIClient
from Core.Creations.DB_access import get_ids_and_srs_ids_by_req_doc_names
from Core.Creations.import_flags import Test
from Utils.Inputs.Common_inputs import inputs

tlc = TestlinkAPIClient(inputs.API_URL, inputs.KEY)
test = Test()

if __name__ == "__main__":
    try:
        existing_projects = tlc.getProjects()
        project_names = [project['name'] for project in existing_projects]
        print("Project Names:\n", project_names)

        enter_project_name = input("Enter project name to assign requirement: ")
        project_id = test.get_project_id(enter_project_name)

        plans = tlc.getProjectTestPlans(project_id)

        plan_ids = [plan['id'] for plan in plans]
        for plan_id in plan_ids:
            print("Plan id : ", plan_id)

        for plan_id in plan_ids:
            print(f"\nFetching test cases for Test Plan ID: {plan_id}")
            existing_test_cases = tlc.getTestCasesForTestPlan(int(plan_id))
            test_case_names = []

            if isinstance(existing_test_cases, dict):
                for key, case_data in existing_test_cases.items():
                    if isinstance(case_data, dict):
                        for platform_id, platform_cases in case_data.items():
                            if isinstance(platform_cases, dict):
                                test_case_names.append(platform_cases.get('tcase_name', 'Unknown'))
                            elif isinstance(platform_cases, list):
                                for test_case in platform_cases:
                                    test_case_names.append(test_case.get('tcase_name', 'Unknown'))
                    elif isinstance(case_data, list):
                        for test_case in case_data:
                            test_case_names.append(test_case.get('tcase_name', 'Unknown'))

            print(test_case_names)
            if test_case_names:
                print("Test Case Names:")
                for name in test_case_names:
                    print(f"- {name}")
            else:
                print("No test cases found for this plan.")

        enter_test_case_name = input("Enter test case name to assign requirement: ")
        test_case_details = tlc.getTestCaseIDByName(
            enter_test_case_name, testprojectname=enter_project_name
        )
        if not test_case_details:
            raise ValueError(f"Test case '{enter_test_case_name}' not found.")

        test_id = test_case_details[0]['id']
        ext_id = tlc.getTestCase(testcaseid=test_id)[0]['full_tc_external_id']

        existing_requirements = tlc.getRequirements(project_id)
        req_doc_ids = [req['req_doc_id'] for req in existing_requirements]
        print("Requirement Documents:")
        for req_id in req_doc_ids:
            print(req_id)

        input_srs_names = input("Enter SRS names separated by commas: ")
        srs_names = [name.strip() for name in input_srs_names.split(",")]

        srs_results = get_ids_and_srs_ids_by_req_doc_names(srs_names)
        if not srs_results:
            raise ValueError(f"No matching SRS names found for: {srs_names}")

        requirements = [
            {'req_spec': srs_id, 'requirements': [req_id]}
            for req_id, srs_id in srs_results
        ]

        response = tlc.assignRequirements(ext_id, project_id, requirements)
        print("Response from TestLink:", response)

    except Exception as e:
        print("Error:", str(e))
