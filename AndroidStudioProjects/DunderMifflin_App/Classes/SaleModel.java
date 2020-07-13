package com.example.dm_app;

public class SaleModel {
    private int entryAdded;
    private String email, clientName;
    private float amount;
    private String branchName;

    public SaleModel(int entryAdded, String email, String clientName, float amount, String branchName) {
        this.entryAdded = entryAdded;
        this.email = email;
        this.clientName = clientName;
        this.amount = amount;
        this.branchName = branchName;
    }

    public SaleModel(){
        this.entryAdded = -1;
        this.email = "Error";
        this.clientName = "Error";
        this.amount = 0;
        this.branchName = "Error";
    }

    public String getBranchName() {
        return branchName;
    }

    public void setBranchName(String branchName) {
        this.branchName = branchName;
    }

    public String getEmail() {
        return email;
    }

    public void setEmail(String email) {
        this.email = email;
    }

    public String getClientName() {
        return clientName;
    }

    public void setClientName(String clientName) {
        this.clientName = clientName;
    }

    public int getEntryAdded() {
        return entryAdded;
    }

    public void setEntryAdded(int entryAdded) {
        this.entryAdded = entryAdded;
    }

    public float getAmount() {
        return amount;
    }

    public void setAmount(float amount) {
        this.amount = amount;
    }

    @Override
    public String toString() {
        return branchName + "\t\t" + email + "\t\t" + clientName + "\t\t" + amount;
    }

    public int CompareClientNames(String a, String b){
        return a.compareTo(b);
    }
}
