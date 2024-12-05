from testlink import TestlinkAPIClient
from Utils.Inputs.Common_inputs import inputs
from Core.Creations.import_flags import Test


class TestLinkManager:
    def __init__(self):
        # Initialize TestLink API client and create an instance of the Test class
        self.tlc = TestlinkAPIClient(inputs.API_URL, inputs.KEY)
        self.test = Test()  # Creating an instance of Test class

    def get_project_id(self, project_name):
        try:
            return self.test.get_project_id(project_name)
        except ValueError as e:
            print(f"Error: {e}")
            return None

    def delete_project(self, project_name):
        """
        Delete the project using its name. If the project is not found, it handles the error.
        """
        # Retrieve project ID based on project name
        project_id = self.get_project_id(project_name)

        if not project_id:
            print(f"Project '{project_name}' not found. Deletion aborted.")
            return

        try:
            # Call TestLink API to delete the project
            response = self.tlc.deleteTestProject(inputs.PROJECT_PREFIX)  # Use project_id here
            print(f"Project '{project_name}' deleted successfully.")
        except Exception as e:
            print(f"Error deleting project '{project_name}': {str(e)}")


if __name__ == '__main__':
    # Initialize TestLinkManager and delete the project
    tlm = TestLinkManager()
    project_name = inputs.PROJECT_NAME  # Replace with the actual project name
    tlm.delete_project(project_name)
