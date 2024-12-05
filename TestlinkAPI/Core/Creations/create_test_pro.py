from openpyxl import load_workbook
from testlink import TestlinkAPIClient
import testlink

from Utils.Inputs.Common_inputs import inputs


class Create:
    tlc = TestlinkAPIClient(inputs.API_URL, inputs.KEY)

    def __init__(self):
        self.newProjectID = 0
        self.newTestPlanID_A = 0
        self.newTestSuiteID_A = 0

    def create_test_project(self):
        newpro = self.tlc.createTestProject(inputs.PROJECT_NAME,inputs.PROJECT_PREFIX,active=1,public=1,notes="pcie based nvme project",options={'requirementsEnabled' : 1, 'testPriorityEnabled' : 1,'automationEnabled' : 1, 'inventoryEnabled' : 1})
        print("Successfully created Test project:  ", newpro)
        self.newProjectID = newpro[0]['id']
        print("project ID --->",self.newProjectID,"\n")
        return self.newProjectID


    def create_test_plan(self):
        newTestPlan = self.tlc.createTestPlan(inputs.PLAN_NAME, testprojectname=inputs.PROJECT_NAME,
                    notes='New TestPlan created with the PCI',active=1, public=1)
        print("Successfully created Test Plan and linked to project:  ", newTestPlan)
        self.newTestPlanID_A = newTestPlan[0]['id']
        print("New Test Plan '%s' - id ---> %s" % (inputs.PLAN_NAME,self.newTestPlanID_A),"\n")
        return self.newTestPlanID_A

    def create_build(self):
        newBuild = self.tlc.createBuild(self.newTestPlanID_A, inputs.BUILD_NAME,
                                    'Notes for the Build', releasedate="2016-12-31")
        print("Successfully created Build", newBuild)
        newBuildID_A = newBuild[0]['id']
        print("New Build '%s' - id ---> %s" % (inputs.BUILD_NAME, newBuildID_A),"\n")
        return newBuildID_A

    def create_platform(self):
        newPlatForm = self.tlc.createPlatform(inputs.PROJECT_NAME, inputs.PLATFORM_NAME,
                notes='Platform for ubuntu, unique name, only used in this project',
                platformondesign=True, platformonexecution=True)
        print("Successfully created Platform", newPlatForm)
        newPlatFormID_A = newPlatForm['id']
        response = self.tlc.addPlatformToTestPlan(self.newTestPlanID_A, inputs.PLATFORM_NAME)
        print("Successfully added platform to test plan...", response , "\n")
        return newPlatFormID_A

    def create_suite(self):
        newTestSuite = self.tlc.createTestSuite(self.newProjectID, inputs.SUITE_NAME,
                    "Details of the Test Suite A")
        print("Successfully created Test Suite", newTestSuite)
        self.newTestSuiteID_A = newTestSuite[0]['id']
        print("New Test Suite '%s' - id ---> %s" % (inputs.SUITE_NAME, self.newTestSuiteID_A))
        return self.newTestSuiteID_A



if __name__ == "__main__":
    create_instance = Create()

    create_instance.create_test_project()
    create_instance.create_test_plan()
    create_instance.create_build()
    create_instance.create_platform()
    create_instance.create_suite()