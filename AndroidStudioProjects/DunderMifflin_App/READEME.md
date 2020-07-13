DUNDER MIFFLIN APP README 7/13/2020
--------------------------------------

NOTE: I SUGGEST WATCHING THE DEMO ON YOUTUBE HERE 
[Youtube Demo](https://www.youtube.com/watch?v=e6lyrYpYxMs)

Contributors
------------
Jason Fiegle

File Listing
------------
MainActivity.java  : Contains the code for showcasing the login page of the application

RegisterActivity.java  : Contains the code for showcasing the registration page of the application

EmployeeActivity.java  : Showcases the main employee page that is shown once signed into the database. This is where most activity will occur, where sales are inputted and where managers can navigate through the different employees of their respective branch.

EmployeeModel.java : Contains the EmployeeModel Object that pulls from the employee table in the SQLite Database and is the object for the user themself.

SaleModel.java :  Contains the SaleModel Object that is used to store sale data in the database (Note: will be improved in the next few days).

EmployeeRecViewAdapter.java : Contains the adapter for the RecyclerView which is shown for managers to see and interact with their employees within their branch.

AddSaleActivity.java: Contains the code for showcasing the add sale page of the application.


Bug Listings
------------
Currently no known bugs

Short Description
-----------------
This Application serves to model how the Dunder Mifflin Paper Company from the Office would function for Salespeople and Managers to use. All relevant data is stored on a database
on the phone using SQLite however in actual practice it should be interfacing with an online database. On the register page you input necessary employee information for the employee object;
passwords must match and emails must be unique from the database otherwise you will be unable to register. Also there can only be one manager for each of the three branches listed, you will be
unable to register as a manager if there already is one for your branch. After registering you are directed to the login page where you type in your email and password which then checks with 
the database to confirm and if such credentials match you are then logged in. Notice I do not implement encryption for passwords yet so they go into the databaseas their initial values however 
this will change. If you are not as manager, right now it is rather boring given salespeople only the ability to enter a simple sale on the employee page. I will be updating the entries for
sales with a spinner containing SKUs and a edittext containing qty for that SKU which will then update a listview for the sale as well as implement a calculation method to auto-determine the
price of the sale, however for now it is basic. In my video demo, which I suggest watching instead of reading the description, I go through a semi-created database with employees to showcase
the manager's perspective. Managers can view all sales from the employees of their branch and sort them with some simple querying shown in a spinner. This will be update for more inputs as
well as for allowing csv tab delimited exporting however that has yet to be implemented. While managers can view the employee pages much like the employees see when they log in, the manager
can not add sales for those employees only themself. each user can also update their profile picture by clicking on it and selecting change image (Note: you must enable the app for storage
permissions on your device). Future implementations will allow for salespeople to update prospects and move potential prospects to leads as well as interface the address column of their leads
with google maps as well as notify the manager that they pulled up certain locations so that the managers can better know what their people are doing throughout the day. Managers will also
have access to the employees prospects and leads and will have similar sorting (querying) options for them as well that can be exported for further visual analysis. Right now the application
is barebones as it has only been a few days however I will update the README and release new videos on the application as it progresses however this is the initial creation.
