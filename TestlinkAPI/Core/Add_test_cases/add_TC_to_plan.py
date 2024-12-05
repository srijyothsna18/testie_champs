from Utils.Inputs.Common_inputs import inputs
from Core.Creations.create_test_pro import Create
from Core.Creations.import_flags import Test
from testlink.testlinkerrors import TLResponseError


class Add:
    def __init__(self):
        self.test = Test()
        self.create = Create()

    def get_test_plan_id(self):
        test_plans = Create.tlc.getProjectTestPlans(self.test.get_project_id(inputs.PROJECT_NAME))
        for test_plan in test_plans:
            if test_plan['name'] == inputs.PLAN_NAME:
                return test_plan['id']
        raise ValueError(f"Test Plan '{inputs.PLAN_NAME}' not found for the given project.")

    def get_platform_id(self):
        id = self.get_test_plan_id()
        platforms = Create.tlc.getTestPlanPlatforms(id)
        if platforms:
            return platforms[0]['id']
        raise ValueError(f"No platforms found for test plan ID...")

    def func(self):
        # Retrieve all test suites for the project
        res = Create.tlc.getFirstLevelTestSuitesForTestProject(self.test.get_project_id(inputs.PROJECT_NAME))
        suite_ids = [suite['id'] for suite in res]
        print("Filtered Suite IDs:", suite_ids)

        pro_id = self.test.get_project_id(inputs.PROJECT_NAME)
        plt_id = self.get_platform_id()
        plan_id = self.get_test_plan_id()

        for suite_id in suite_ids:
            r = Create.tlc.getTestCasesForTestSuite(testsuiteid=suite_id, deep=False, details="simple")
            #print(f"Test cases for suite {suite_id}: {r}")

            for test_case in r:
                tc_id = test_case["id"]
                tc_full_ext_id = Create.tlc.getTestCase(testcaseid=tc_id)[0]["full_tc_external_id"]
                #print(f"Test case {tc_id} External ID: {tc_full_ext_id}")

                try:
                    response = Create.tlc.addTestCaseToTestPlan(
                        testprojectid=pro_id,
                        testplanid=plan_id,
                        testcaseexternalid=tc_full_ext_id,
                        version=1,
                        platformid=plt_id
                    )
                    print(f"Response for test case {tc_full_ext_id} added to test plan: {response}")

                except TLResponseError as e:
                    if "3045" in str(e):
                        print(f"Test case {tc_full_ext_id} is already linked to the test plan '{inputs.PLAN_NAME}' for platform ID {plt_id}. Skipping.")
                    else:
                        raise

if __name__ == "__main__":
    a = Add()
    a.func()
