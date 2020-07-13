package com.example.dm_app;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

public class EmployeeActivity extends AppCompatActivity implements EmployeeRecViewAdapter.OnEmployeeListener {


    private RecyclerView employeeRecyclerView;
    private ArrayList<EmployeeModel> employees;
    private EmployeeModel currentEmployee;
    private TextView txtName, txtEmail, txtBranch, txtStatus;
    private ImageView imgProfile;
    private Spinner spnSort;
    private ListView lstSales;
    private Button btnAdd, btnChange;
    ArrayAdapter salesArrayAdapter;
    DataBaseHelper dataBaseHelper;
    String changeEmp;
    String OriginalEmail;
    RelativeLayout EmployeeLayout;
    private static final int PICK_IMAGE = 100;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if(getIntent().getExtras() != null){
            Bundle extras = getIntent().getExtras();
            final String Email = getIntent().getExtras().getString("Email");
            changeEmp = extras.getString("NewEmail");
            OriginalEmail = Email;
        }else{
            final String Email = "";
            OriginalEmail = Email;
            changeEmp = Email;
        }


        //String Pass = getIntent().getExtras().getString("Pass");
        setContentView(R.layout.activity_employee);

        initialize();
        displayEmployees(changeEmp);
        ArrayList<String> sortItems = new ArrayList<>();
        sortItems.add("Default");
        sortItems.add("Sort By Client");
        sortItems.add("Sort by Amount: Low-To-High");
        sortItems.add("Sort by Amount: High-To-Low");
        if(currentEmployee.isManager()){
            sortItems.add("Sort by Employee");
        }

        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_dropdown_item, sortItems);
        spnSort.setAdapter(adapter);
        spnSort.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                int SortID = i;
                ShowAllSalesOnListView(currentEmployee,SortID);
                Toast.makeText(EmployeeActivity.this, spnSort.getSelectedItem().toString() + " Selected", Toast.LENGTH_SHORT).show();
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
        dataBaseHelper = new DataBaseHelper(EmployeeActivity.this);

        if(dataBaseHelper.imageExists(changeEmp)){
            imgProfile.setImageBitmap(dataBaseHelper.getImage(currentEmployee.getEmail()));
        }

        if(OriginalEmail.equals(changeEmp)){
            btnAdd.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    Intent intent = new Intent(EmployeeActivity.this, AddSaleActivity.class);
                    intent.putExtra("Email", OriginalEmail);
                    intent.putExtra("Branch", currentEmployee.convertBranch(currentEmployee.getBranch()));
                    startActivity(intent);
                }
            });
        }else{
            btnAdd.setVisibility(View.GONE);
        }
        imgProfile.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                btnChange.setVisibility(View.VISIBLE);
            }
        });

        EmployeeLayout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                btnChange.setVisibility(View.GONE);
            }
        });
        btnChange.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(Intent.ACTION_PICK,Uri.parse(
                        "content://media/internal/images/media"
                ));
                startActivityForResult(intent, PICK_IMAGE);
            }
        });


    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if(resultCode == RESULT_OK && requestCode == PICK_IMAGE){
            Uri uri = data.getData();
            String x = getPath(uri);
            boolean success = dataBaseHelper.imageExists(currentEmployee.getEmail());
            boolean worked;
            if(success){
                worked = dataBaseHelper.updateImage(currentEmployee.getEmail(),x);

            }
            else{
                worked = dataBaseHelper.addImage(currentEmployee.getEmail(),x);
            }

            if(worked){
                Toast.makeText(this, "Succesfully Added Image", Toast.LENGTH_SHORT).show();
                refresh(currentEmployee.getEmail());

            }
            else{
                Toast.makeText(this, "Failed to Update Profile", Toast.LENGTH_SHORT).show();
            }

        }
    }

    public String getPath(Uri uri){
        if(uri == null){
            return null;
        }
        String[] projection = {MediaStore.Images.Media.DATA};
        Cursor cursor = managedQuery(uri, projection, null, null, null);
        if(cursor != null){
            int column_index = cursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
            cursor.moveToFirst();
            return cursor.getString(column_index);
        }
        return uri.getPath();
    }


    private void ShowAllSalesOnListView(EmployeeModel currentE, int SortID) {
        System.out.println("Passed in : " + SortID);

        switch(SortID){
            case 0:
                //System.out.println("WITHIN DEFAULT");
                salesArrayAdapter = new ArrayAdapter<SaleModel>(EmployeeActivity.this, R.layout.mytextview, dataBaseHelper.getAllSales(currentEmployee));
                lstSales.setAdapter(salesArrayAdapter);
                break;
            case 1:
                //System.out.println("In Sort By Client");
                salesArrayAdapter = new ArrayAdapter<SaleModel>(EmployeeActivity.this, R.layout.mytextview, dataBaseHelper.getAllSalesByClient(currentEmployee));
                lstSales.setAdapter(salesArrayAdapter);
                break;
            case 2:
                salesArrayAdapter = new ArrayAdapter<SaleModel>(EmployeeActivity.this, R.layout.mytextview, dataBaseHelper.getAllSalesByAmount_LTH(currentEmployee));
                lstSales.setAdapter(salesArrayAdapter);
                break;
            case 3:
                salesArrayAdapter = new ArrayAdapter<SaleModel>(EmployeeActivity.this, R.layout.mytextview, dataBaseHelper.getAllSalesByAmount_HTL(currentEmployee));
                lstSales.setAdapter(salesArrayAdapter);
                break;
            case 4:
                salesArrayAdapter = new ArrayAdapter<SaleModel>(EmployeeActivity.this, R.layout.mytextview, dataBaseHelper.getAllSalesByEmployee(currentEmployee));
                lstSales.setAdapter(salesArrayAdapter);
                break;
            case 5:
                salesArrayAdapter = new ArrayAdapter<SaleModel>(EmployeeActivity.this, R.layout.mytextview, dataBaseHelper.getAllSalesByBranchByID(currentEmployee));
                lstSales.setAdapter(salesArrayAdapter);
            default:
                if(currentEmployee.isManager()){
                    salesArrayAdapter = new ArrayAdapter<SaleModel>(EmployeeActivity.this, R.layout.mytextview, dataBaseHelper.getAllSalesByBranchByID(currentEmployee));
                    lstSales.setAdapter(salesArrayAdapter);
                }
                break;
        }

    }

    public void refresh(String email){
        Intent intent = new Intent(EmployeeActivity.this, EmployeeActivity.class);
        Bundle bundle = new Bundle();
        bundle.putString("Email", OriginalEmail);
        bundle.putString("NewEmail", email);
        intent.putExtras(bundle);
        startActivity(intent);
        EmployeeActivity.this.finish();
    }
    @Override
    public void onEmployeeClick(int position) {
        Toast.makeText(this, employees.get(position).getFirstName() + " Clicked", Toast.LENGTH_SHORT).show();
        Intent intent = new Intent(EmployeeActivity.this, EmployeeActivity.class);
        Bundle bundle = new Bundle();
        bundle.putString("Email", OriginalEmail);
        bundle.putString("NewEmail", employees.get(position).getEmail());
        intent.putExtras(bundle);
        startActivity(intent);
        EmployeeActivity.this.finish();

    }

    public void initialize() {
        employeeRecyclerView = findViewById(R.id.employeeRecyclerView);
        txtName = findViewById(R.id.txtName);
        txtEmail = findViewById(R.id.txtEmail);
        txtBranch = findViewById(R.id.txtBranch);
        txtStatus = findViewById(R.id.txtStatus);
        imgProfile = findViewById(R.id.imgProfile);
        spnSort = findViewById(R.id.spnSort);
        lstSales = findViewById(R.id.lstSales);
        btnAdd = findViewById(R.id.btnAdd);
        btnChange = findViewById(R.id.btnChange);
        EmployeeLayout = findViewById(R.id.EmployeeLayout);

    }

    public void displayEmployees(String email) {
        employees = new ArrayList<>();
        DataBaseHelper dataBaseHelper = new DataBaseHelper(EmployeeActivity.this);
        currentEmployee = dataBaseHelper.getEmployee(email);
        txtName.setText(currentEmployee.getFirstName() + " " + currentEmployee.getLastName());
        txtEmail.setText(currentEmployee.getEmail());
        txtBranch.setText(dataBaseHelper.convertBranch(currentEmployee.getBranch()));


        if (currentEmployee.isManager()) {
            txtStatus.setText("Manager");
            employees = (ArrayList<EmployeeModel>) dataBaseHelper.getEveryoneInBranch(currentEmployee.getBranch());

            EmployeeRecViewAdapter adapter = new EmployeeRecViewAdapter(this, this);
            adapter.setEmployees(employees);

            employeeRecyclerView.setAdapter(adapter);
            employeeRecyclerView.setLayoutManager(new GridLayoutManager(this, 1, RecyclerView.HORIZONTAL, false));
        } else {
            EmployeeRecViewAdapter adapter = new EmployeeRecViewAdapter(this, this);
            employees = (ArrayList<EmployeeModel>) dataBaseHelper.getNone();
            adapter.setEmployees(employees);

            employeeRecyclerView.setAdapter(adapter);
            employeeRecyclerView.setLayoutManager(new GridLayoutManager(this, 1, RecyclerView.HORIZONTAL, false));
            txtStatus.setText("Employee");
        }

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.example_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        switch (item.getItemId()) {
            case R.id.itemProfile:
                Intent intent = new Intent(EmployeeActivity.this, EmployeeActivity.class);
                Bundle bundle = new Bundle();
                bundle.putString("Email", OriginalEmail);
                bundle.putString("NewEmail", OriginalEmail);
                intent.putExtras(bundle);
                startActivity(intent);
                EmployeeActivity.this.finish();
                break;
            case R.id.logout:
                Intent Myintent = new Intent(EmployeeActivity.this, MainActivity.class);
                startActivity(Myintent);

        }
        return super.onOptionsItemSelected(item);
    }

}
