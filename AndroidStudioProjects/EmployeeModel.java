package com.example.dm_app;

public class EmployeeModel {
    private int branch;
    private String firstName, lastName, email, pass;
    private boolean isManager;

    public EmployeeModel(String firstName, String lastName, String email, String pass, boolean isManager, int branch) {
        this.firstName = firstName;
        this.lastName = lastName;
        this.email = email;
        this.pass = pass;
        this.isManager = isManager;
        this.branch = branch;
    }
    public EmployeeModel(){
        this.firstName = "Error";
        this.lastName = "Error";
        this.email = "Error";
        this.pass = "Error";
        this.isManager = false;
        this.branch = -1;
    }

    //GETTERS AND SETTERS


    public int getBranch() {
        return branch;
    }

    public void setBranch(int branch) {
        this.branch = branch;
    }

    public String getFirstName() {
        return firstName;
    }

    public void setFirstName(String firstName) {
        this.firstName = firstName;
    }

    public String getLastName() {
        return lastName;
    }

    public void setLastName(String lastName) {
        this.lastName = lastName;
    }

    public String getEmail() {
        return email;
    }

    public void setEmail(String email) {
        this.email = email;
    }

    public String getPass() {
        return pass;
    }

    public void setPass(String pass) {
        this.pass = pass;
    }


    public boolean isManager() {
        return isManager;
    }

    public void setManager(boolean manager) {
        isManager = manager;
    }

    //TO STRING


    @Override
    public String toString() {
        return "EmployeeModel{" +
                "branch=" + branch +
                ", firstName='" + firstName + '\'' +
                ", lastName='" + lastName + '\'' +
                ", email='" + email + '\'' +
                ", pass='" + pass + '\'' +
                ", isManager=" + isManager +
                '}';
    }
    public String convertBranch(int branch){
        if(branch == 1){
            return "Scranton";
        }else if(branch == 2){
            return "Stamford";
        }else if(branch == 3){
            return "Nashua";
        }else{
            return "Unknown";
        }
    }
}
