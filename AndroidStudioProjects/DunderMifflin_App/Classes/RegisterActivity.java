package com.example.dm_app;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.Toast;



public class RegisterActivity extends AppCompatActivity implements AdapterView.OnItemSelectedListener {

    private Button btnRegister;
    private EditText edtFirst, edtLast, edtEmail, edtPass, edtPassConfirm;
    private Switch swManager;
    private Spinner spnBranch;
    private int branch;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_register);

        btnRegister = findViewById(R.id.btnRegister);
        edtFirst = findViewById(R.id.edtFirst);
        edtLast = findViewById(R.id.edtLast);
        edtEmail = findViewById(R.id.edtEmail);
        edtPass = findViewById(R.id.edtPass);
        edtPassConfirm = findViewById(R.id.edtPassConfirm);
        swManager = findViewById(R.id.swManager);
        spnBranch = findViewById(R.id.spnBranch);
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(this, R.array.branch, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spnBranch.setAdapter(adapter);
        spnBranch.setOnItemSelectedListener(this);

        btnRegister.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                EmployeeModel employeeModel = null;
                //Intent intent = new Intent(RegisterActivity.this, MainActivity.class);
                if(edtPass.getText().toString().equals(edtPassConfirm.getText().toString())){
                    try{
                        employeeModel = new EmployeeModel(edtFirst.getText().toString(), edtLast.getText().toString(), edtEmail.getText().toString(),edtPass.getText().toString(), swManager.isChecked(), branch);
                        //Toast.makeText(RegisterActivity.this, "Move to Login Page", Toast.LENGTH_SHORT).show();
                    }
                    catch(Exception e){
                        Toast.makeText(RegisterActivity.this, "Error creating Employee", Toast.LENGTH_SHORT).show();
                        employeeModel = new EmployeeModel("error", "error", "error", "error", false, -1);
                    }

                    DataBaseHelper dataBaseHelper = new DataBaseHelper(RegisterActivity.this);
                    boolean success = dataBaseHelper.addOne(employeeModel);
                    if(success == true){
                        Toast.makeText(RegisterActivity.this, "Registered Successfully", Toast.LENGTH_LONG).show();
                        Intent intent = new Intent(RegisterActivity.this, MainActivity.class);
                        startActivity(intent);
                    }else{
                        if(dataBaseHelper.managerForBranch(employeeModel.isManager(),employeeModel.getBranch()) == false){
                            Toast.makeText(RegisterActivity.this, "There is already a manager for the " + dataBaseHelper.convertBranch(employeeModel.getBranch()) + " Branch", Toast.LENGTH_LONG).show();
                        }else {
                            Toast.makeText(RegisterActivity.this, "Failed to Register", Toast.LENGTH_LONG).show();
                        }
                    }

                }else{
                    Toast.makeText(RegisterActivity.this, "Error: Passwords do not match", Toast.LENGTH_SHORT).show();

                }




            }
        });
    }

    @Override
    public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
        String text = adapterView.getItemAtPosition(i).toString();
        if(text.contains("Scranton")){
            branch = 1;
        }
        else if(text.contains("Stamford")){
            branch = 2;
        }
        else if(text.contains("Nashua")){
            branch = 3;
        }else{
            branch = -1;
        }
    }

    @Override
    public void onNothingSelected(AdapterView<?> adapterView) {

    }
}